import pandas as pd
import numpy as np

# Config
data = "sp500_data.csv"
price = "close"
ticker = "Name"
date = "date"

# Load and format data
df = pd.read_csv(data, parse_dates=[date])
price_df = df.pivot(index=date, columns=ticker, values=price)

# Calculate ave daily log returns and covariance matrix
returns = np.log(price_df / price_df.shift(1)).dropna(how="all")
mean_returns = returns.mean()
cov_matrix = returns.cov()

# Annualise
trading_days = 252
mean_returns_annual = mean_returns * trading_days
cov_matrix_annual = cov_matrix * trading_days

print("Mean annualized returns:")
print(mean_returns_annual)

print("\nCovariance matrix (annualized):")
print(cov_matrix_annual)

mean_returns_annual.to_csv("mean_returns.csv")
cov_matrix_annual.to_csv("covariance_matrix.csv")
