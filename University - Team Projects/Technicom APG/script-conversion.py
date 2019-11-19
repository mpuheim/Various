# simple script to test validity of file conversions

from modules import utils, fileio

# load customers table as CSV
tab = fileio.loadTable('data/customers.csv')
# save customers table as JSON
fileio.saveTable(tab,'customers.json')
# load customers table as JSON
tab = fileio.loadTable('customers.json')
# save customers table as CSV
fileio.saveTable(tab,'customers.csv')

# load customers table as CSV
tab = fileio.loadTable('customers.csv')
# save customers table as JSON
fileio.saveTable(tab,'customers_2.json')
# load customers table as JSON
tab = fileio.loadTable('customers_2.json')
# save customers table as CSV
fileio.saveTable(tab,'customers_2.csv')


# load purchases table as CSV
tab = fileio.loadTable('data/purchases.csv')
# save purchases table as JSON
fileio.saveTable(tab,'purchases.json')
# load purchases table as JSON
tab = fileio.loadTable('purchases.json')
# save purchases table as CSV
fileio.saveTable(tab,'purchases.csv')

# load purchases table as CSV
tab = fileio.loadTable('purchases.csv')
# save purchases table as JSON
fileio.saveTable(tab,'purchases_2.json')
# load purchases table as JSON
tab = fileio.loadTable('purchases_2.json')
# save purchases table as CSV
fileio.saveTable(tab,'purchases_2.csv')
