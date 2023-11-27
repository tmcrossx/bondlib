# Tax Loss Harvesting

## Tables

### secmaster

Security master database.

```sql
CREATE TABLE secmaster (
    CUSIP char(9) PRIMARY KEY
    issue_date DATE,
    dated_date DATE,
    maturity_date DATE NOT NULL,
    coupon REAL NOT NULL,
    frequency INT NOT NULL,
    day_count_basis INT NOT NULL,
    ...
);
```

Auxiliary tables: `frequency`, `day_count_basis`

### portfolio

All available positions.

```sql
CREATE TABLE positions (
    CUSIP char(9) NOT NULL,
    clientId INT NOT NULL,
    portfolioID INT NOT NULL,
    ...
    PRIMARY KEY (clientId, portfolioId, CUSIP)
);

Auxiliary tables: `clientId`, `portfolioId`
```

## Rules

Every `clientId` and `portfolioId` pair is associated with a set of _rules_.  
Matching happens in two stages. First the fields
and parameters for a `CUSIP` are queried based on rules. 
Next the matching rules are applied to the portfolio of available matches.

### Analytic rules

E.g., absolute yield difference less than or equal to 10 basis points

| name | type | function | argument | comparison |
|-|-|-|-|-|
|absolute_difference_yield|analytic|absolute_difference|yield|le|

Fields
```sql
SELECT CUSIP, yield
FROM secmaster
WHERE CUSIP = @CUSIP
```
Parameters
```sql
SELECT absolute_difference_yield
FROM rules
WHERE portfolioId = @portfolioId AND clientId = @clientId
```
Analytics
```sql
SELECT ABS(@yield - yield) AS absolute_difference_yield  
WHERE absolute_difference_yield <= @absolute_difference_yield
```

### Category rules

E.g., use of proceeds both tobacco 

```sql
SELECT useOfProceedsGroup
```

### Logical rules

E.g., clients must be different (no crossing)
