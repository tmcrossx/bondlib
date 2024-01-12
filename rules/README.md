# CrossX

Collect _client_ _portfolio_ _positions_ that are candidates
for trading. Clients specify _matching rules_ that can apply at
the position or _portfolio_ level.

Given a position generate all positions _matching_ at the position level. 

Generate a _bundle_ of positions matching at the portfolio level.

## Tables

### [clients](clients.sql)

### [positions](positions.sql)

Clients send a table of positions having a
portfolio and a position identifier, CUSIP,
quantity. If they want us to calculate tax loss
then include the purchase date and purchase price.


Portfolio and position identify a unique lot held
by the customer. 

Auxiliary tables: `clientId`, `portfolioId`, `positionId`
```

### Matching rules

Given clientId, portfolioId, and positionId query all
data required for subsequent operations.

### security_master

Security master database.

`security_master/payload/instrument/debt/fixed_income/`

|XPATH|type|
| -:|:-|
|bond_class|BondClass|
|daycount_basis_type|ApexDaycountBasisType|
|first_coupon_date|date|
|interest_payment_frequency|FrequencyType|
|issue_amount|decimal|
|maturity_date|date|
|nominal_value|decimal|
|orig_principal_amount|decimal|
|original_coupon_rate|decimal|
|outstanding_amount|decimal|
|call_indicator|boolean|
|first_payment_date|date|
|interest_calc_method|InterestCalcMethods|
|accrued_roll_convention|ApexRollConvention|
|payment_roll_convention|ApexRollConvention|
|original_maturity_date|date|
|subordination_type|GsmSubordinationType|

`security_master/payload/instrument/debt/muni_details/`

|XPATH|type|description|
| -:|:-|:-|
|capital_type|MuniCapitalType|
|conduit_obligor_name|string[integer]|
|issue_key|integer|
|issue_text|string|
|muni_issue_type|MuniSupplementalIssueType|
|muni_security_type|MuniSecurityType|The revenues and assets available to service the payments and debt repayment of the security identified by the instrument_id.|
|state_tax_status|MuniStateTaxType|If the bond is subject to state taxes in the home state of the issuer.|
|use_of_proceeds|MuniUseOfProceedsType|
|secured|MuniSecuredType|
|sec_regulation|MuniSecRegulation|
|purpose_class|MuniPurposeClassType|
|purpose_sub_class|MuniPurposeSubClassType|

security_master/payload/instrument/debt/call_details/call_schedule  
security_master/payload/instrument/debt/call_details/call_schedule/call_date  

security_master/payload/instrument/debt/muni_associated_obligor  

```sql
CREATE TABLE security_master (
    CUSIP char(9) PRIMARY KEY
    issue_date DATE,
    accrual_date DATE, -- ?
    dated_date DATE,
    maturity_date DATE NOT NULL,
    coupon REAL NOT NULL,
    frequency INT NOT NULL,
    daycount_basis_type
    day_count_basis INT NOT NULL,
    business_day_convention_code INT NOT NULL,
    use_of_proceeds INT NOT NULL,
    ...
);
```

Auxiliary tables: `frequency`, `day_count_basis`, `business_day_convention_code`, `use_of_proceeds`



### obligor

    -- [LEI](https://www.gleif.org/en/about-lei-introducing-the-legal-entity-identifier-lei)

```sql
CREATE TABLE obligor (
    obligorId INT NOT NULL PRIMARY KEY,

    LEI char[20],
    name TEXT -- String containing obligor name
    -- other ids to map to ???
)
```

```sql
CREATE TABLE obligor_subsidiary (
    obligorId INT NOT NULL
    subsidiary INT NOT NULL
)

```

## Rules

Every `clientId` and `portfolioId` pair is associated with a set of _rules_.  
Matching happens in stages to successively eliminate non-candidates. 
First the fields and parameters for a `CUSIP` are queried based on rules. 
Next the matching rules are applied to the portfolio of available matches.

### Portfolio

Limits on portfolio aggregates.

E.g., cash position must be less that 20%

```sql
SELECT (SUM(a.quantity FROM portfolio WHERE ...) AS a
      ,(SUM(b.quantity FROM portfolio WHERE ...) AS b
      , a/b as ab -- ?
WHERE ab <= 0.03
```

### Analytic rules

E.g., absolute yield difference less than or equal to 10 basis points

| name | type | function | argument | comparison |
|-|-|-|-|-|
|absolute_difference_yield|analytic|absolute_difference|yield|le|

Fields
```sql
SELECT CUSIP, yield
FROM security_master
WHERE CUSIP = @CUSIP
```
Parameters
```sql
SELECT absolute_difference_yield
FROM rules_parameters
WHERE portfolioId = @portfolioId AND clientId = @clientId
```
Analytics
```sql
SELECT ABS(@yield - yield) AS absolute_difference_yield  
--WHERE absolute_difference_yield <= @absolute_difference_yield
ORDER BY absolute_difference_yield ASC
```

### Category rules

E.g., use of proceeds both tobacco 

```sql
SELECT useOfProceedsGroup
```

### Logical rules

E.g., clients must be different (no crossing)

```sql
WHERE @clientId <> clientId
```
