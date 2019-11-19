from modules import utils, fileio
from modules.database import Database
from modules.generator import Generator
from modules.delimiter import Delimiter

# load tables
customers = fileio.loadTable('data/customers.json')
purchases = fileio.loadTable('data/purchases.json')
# create database
database = Database([customers,purchases],'customer_id')
# generate new attributes
generator = Generator(database)
generator.generate()
# eliminate attributes with low correlation with the target attribute
target_attribute = 'properties.birth_year'
safe_list = ['properties.first_name','properties.last_name','properties.gender']
delimiter = Delimiter(database)
delimiter.eliminate(target_attribute,safe_list)
# save updated customer table
fileio.saveTable(database.customers,'customers_new.csv')
