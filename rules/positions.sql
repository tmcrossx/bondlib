CREATE TABLE positions (
    clientId INT NOT NULL,
    portfolioId INT NOT NULL,
    positionId INT NOT NULL,
    CUSIP char(9) NOT NULL,
    quantity decimal,
    purchase_date date,
    purchase_price decimal,
    --minQuantity decimal, -- goes into rules???
    ...
    PRIMARY KEY (clientId, portfolioId, positionId) -- ??? ensure positionId is unique
);
