from sys import exc_clear
import tests
import math

class Table:
    # constructor
    def __init__(self,name = 'N/A',head=list(),body=list(),types=list()):
        self.name = name
        self.head = head
        self.types = types
        self.body = body
        if len(self.types)==0:
            self.determineTypes();
        
    # returns single attribute value from the table
    def getAttributeValue(self,col,row):
        return self.body[row][col]
        
    # return list of all attribute values within the table
    def getAttributeValues(self,col):
        return list(row[col] for row in self.body)
    
    # return the type of the attribute in the given column
    def getAttributeType(self,col):
        return self.types[col]
    
    # returns the name of the attribute in the given column
    def getAttributeName(self,col):
        return self.head[col]
        
    # returns the index of the attribute with the given name
    def getAttributeColumn(self,name):
        return self.head.index(name)
    
    # remove attribute with given index from the table
    def removeAttribute(self,col):
        self.head.pop(col)
        self.types.pop(col)
        for row in self.body:
            row.pop(col)
        
    # automatically determines the type of the attributes in all columns
    def determineTypes(self):
        # reset types
        self.types = list()
        # for each attribute (each column in the table)
        for i in range(0,len(self.head)):
            # determine if all values can be converted into number
            success = 1;
            for val in list(row[i] for row in self.body):
                # skip undefined values
                if val != '':
                    try:
                        float(val) # try to convert to number
                    except ValueError:
                        exc_clear() # don't show error if conversion fails
                        success = 0;
                        break
            # determine type according to the result
            if success == 1:
                self.types.append('numeric')
            else:
                self.types.append('nominal')

    # convert numeric attribute to nominal
    def convertToNominal(self,attribute,levels=3):
        # get the attribute index
        if type(attribute) is str:
            index = self.getAttributeColumn(attribute)
        elif type(attribute) is int:
            if attribute >= len(self.head):
                raise ValueError('value of the attribute index is out of bounds of the attribute list')
            index = attribute
        else:
            raise ValueError('invalid type of argument: attribute')
        # no conversion if target type is already nominal
        if self.types[index]=='nominal':
            return
        # get list of all attribute values
        original_list = self.getAttributeValues(index)
        # get list of values converted to float omitting missing values
        available_list = map(float,[val for val in original_list if len(val)!=0])
        # calculate mean
        mean = sum(available_list)/float(len(available_list))
        # calculate standard deviation
        variance = map(lambda x: (x - mean)**2, available_list)
        average_variance = sum(variance)/float(len(variance))
        standard_deviation = math.sqrt(average_variance)
        # calculate z-score list
        zscore_list = list()
        for val in self.getAttributeValues(index):
            if len(val)==0:
                zscore_list.append('')
            else:
                zscore_list.append((float(val)-mean)/standard_deviation)
        # set max and min values
        max = mean+standard_deviation
        min = mean-standard_deviation
        # generate namespace for nominal values
        add_low = 1
        add_med = 1
        add_hig = 1
        name_list = list()
        if levels < 2:
            raise ValueError('value of the attribute need to be at least 2: levels')
        else:
            for i in range(0,levels):
                if float(i) < float(levels)/3:
                    name_list.append('low' + (str(add_low) if levels > 3 else ''))
                    add_low += 1
                elif float(i) > 2*float(levels)/3-1:
                    name_list.append('high' + (str(add_hig) if levels > 3 else ''))
                    add_hig += 1
                else:
                    name_list.append('med' + (str(add_med) if levels > 7 else ''))
                    add_med += 1
        # convert values
        value_list = list()
        for val in zscore_list:
            if type(val) is str:
                value_list.append('')
            elif float(val)-min > 2*standard_deviation:
                value_list.append(name_list[levels-1])
            else:
                for i in range(0,levels):
                    if float(val)-min <= (i+1)*(max-min)/levels:
                        value_list.append(name_list[i])
                        break
        # create attribute
        self.head.append(self.head[index]+'_nominal')
        self.types.append('nominal')
        val = iter(value_list)
        for row in self.body:
            row.append(next(val))
    
    # calculate corellation of two attributes
    def correlation(self,attr1,attr2):
        # get the index of the first attribute
        if type(attr1) is str:
            col1 = self.getAttributeColumn(attr1)
        elif type(attr1) is int:
            if attr1 >= len(self.head):
                raise ValueError('value of the attribute index is out of bounds of the attribute list')
            col1 = attr1
        else:
            raise ValueError('invalid type of argument: attribute')
        # get the index of the second attribute
        if type(attr2) is str:
            col2 = self.getAttributeColumn(attr2)
        elif type(attr2) is int:
            if attr2 >= len(self.head):
                raise ValueError('value of the attribute index is out of bounds of the attribute list')
            col2 = attr2
        else:
            raise ValueError('invalid type of argument: attribute')
        # calculate correlation using test according to the type of the attribute
        if self.getAttributeType(col1) == 'nominal':
            if self.getAttributeType(col2) == 'nominal':
                return tests.chiSqTest(self.getAttributeValues(col1),self.getAttributeValues(col2))
            elif self.getAttributeType(col2) == 'numeric':
                attr1_values = self.getAttributeValues(col1)
                name_count = len(set(attr1_values))
                name_count = 2 if name_count<2 else name_count
                self.convertToNominal(col2,name_count)
                attr2_values = self.getAttributeValues(len(self.head)-1)
                result = tests.chiSqTest(attr1_values,attr2_values)
                self.removeAttribute(len(self.head)-1)
                return result
            else:
                raise ValueError('could not determine the type of the attribute within the table; use determineTypes() method first')
        elif self.getAttributeType(col1) == 'numeric':
            if self.getAttributeType(col2) == 'numeric':
                return tests.pearsonTest(self.getAttributeValues(col1),self.getAttributeValues(col2))
            if self.getAttributeType(col2) == 'nominal':
                attr2_values = self.getAttributeValues(col2)
                name_count = len(set(attr2_values))
                name_count = 2 if name_count<2 else name_count
                self.convertToNominal(col1,name_count)
                attr1_values = self.getAttributeValues(len(self.head)-1)
                result = tests.chiSqTest(attr1_values,attr2_values)
                self.removeAttribute(len(self.head)-1)
                return result
            else:
                raise ValueError('could not determine the type of the attribute within the table; use determineTypes() method first')
        else:
            raise ValueError('could not determine the type of the attribute within the table; use determineTypes() method first')
        