CREATE TABLE security_master (
    CUSIP char(9) PRIMARY KEY
    issue_date DATE,
    issue_amount DECIMAL,
    outstanding_amount DECIMAL,
    accrual_date DATE, -- ?
    dated_date DATE,
    maturity_date DATE NOT NULL,
    original_maturity_date DATE,
    coupon REAL NOT NULL,
    interest_payment_frequency INT NOT NULL,
    daycount_basis_type INT NOT NULL,
    business_day_convention_code INT NOT NULL,
    use_of_proceeds INT NOT NULL,
    -- ...
);