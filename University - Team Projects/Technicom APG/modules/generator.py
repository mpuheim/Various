import itertools, collections

class Generator:
    # constructor
    def __init__(self,database):
        self.database = database
        self.created_limit = 0
        self.created_total = 0
        
    # generate attributes
    def generate(self,count=100):
        self.created_limit=self.created_total+count
        self.genNumericAggregations()
        self.genNominalAggregations()
        
    # aggregation (count,sum,average,max,min,variance) generator for numeric event attributes
    def genNumericAggregations(self):
        # check event data
        if self.database.events == None:
            raise ValueError('cannot generate aggregated attributes without event data tables')
        # create aggregation functions name list
        aggr_names=['count','sum','average','max','min','variance']
        # get the key index in the customer table
        customer_key_index = self.database.customers.getAttributeColumn(self.database.key)
        # generate aggregations across all event tables
        for event_tab in self.database.events:
            # get the key index in the event table
            event_key_index = event_tab.getAttributeColumn(self.database.key)
            # generate aggregations for all numeric attributes
            for attr, type in itertools.izip(event_tab.head,event_tab.types):
                if type == 'numeric' and attr != self.database.key:
                    # get the key list in the event table
                    keys = event_tab.getAttributeValues(event_key_index)
                    # get the attribute index in event table
                    event_att_index = event_tab.getAttributeColumn(attr)
                    # get the value list in the event table
                    values = event_tab.getAttributeValues(event_att_index)
                    # create temporary dictionary
                    d = dict()
                    for key,value in itertools.izip(keys,values):
                        if key in d:
                            d[key].append(value)
                        else:
                            d[key]=[value]
                    # create metadata
                    temp_counter = 0
                    ban_list=[0,0,0,0,0,0]
                    for name, i in zip(aggr_names,range(len(aggr_names))):
                        full_name = event_tab.head[event_att_index]+'.'+name
                        # generate only requisite number of attributes
                        if self.created_total+temp_counter > self.created_limit:
                            break
                        # check for duplicity
                        elif full_name in self.database.customers.head:
                            ban_list[i]=1
                            continue
                        else:
                            # create names of new attributes in header of customer table
                            self.database.customers.head.append(full_name)
                            # set datatype
                            self.database.customers.types.append('numeric')
                            # increase attribute iterator
                            temp_counter+=1
                    # generate data
                    for customer in self.database.customers.body:
                        # help variables
                        key = customer[customer_key_index]
                        all_values = d[key] if key in d else []
                        float_values = [float(val) for val in all_values if len(val)>0]
                        count = len(all_values)
                        countf = len(float_values)
                        summ = sum(float_values)
                        if countf>0:
                            average = summ/float(countf)
                            maxval = max(float_values)
                            minval = min(float_values)
                            variance = maxval-minval
                        else:
                            average = ''
                            maxval = ''
                            minval = ''
                            variance = ''
                        # append data
                        if self.created_total+1 <= self.created_limit and ban_list[0]==0:
                            # count
                            customer.append(str(count))
                        if self.created_total+2 <= self.created_limit and ban_list[1]==0:
                            # sum                     
                            customer.append(str(summ))
                        if self.created_total+3 <= self.created_limit and ban_list[2]==0:
                            # average
                            customer.append(str(average))
                        if self.created_total+4 <= self.created_limit and ban_list[3]==0:
                            # max 
                            customer.append(str(maxval))
                        if self.created_total+5 <= self.created_limit and ban_list[4]==0:
                            # min
                            customer.append(str(minval))
                        if self.created_total+6 <= self.created_limit and ban_list[5]==0:
                            # variance
                            customer.append(str(variance))
                    # increase attribute iterator
                    self.created_total+=temp_counter
    
    # aggregation (count,unique_count,favourite_frequency,favourite_item) generator for nominal event attributes
    def genNominalAggregations(self):
        # check event data
        if self.database.events == None:
            raise ValueError('cannot generate aggregated attributes without event data tables')
        # create aggregation functions name list
        aggr_names=['count','unique_count','favourite_frequency','favourite_item']
        # get the key index in the customer table
        customer_key_index = self.database.customers.getAttributeColumn(self.database.key)
        # generate aggregations across all event tables
        for event_tab in self.database.events:
            # get the key index in the event table
            event_key_index = event_tab.getAttributeColumn(self.database.key)
            # generate aggregations for all nominal attributes
            for attr, type in itertools.izip(event_tab.head,event_tab.types):
                if type == 'nominal' and attr != self.database.key:
                    # get the key list in the event table
                    keys = event_tab.getAttributeValues(event_key_index)
                    # get the attribute index in event table
                    event_att_index = event_tab.getAttributeColumn(attr)
                    # get the value list in the event table
                    values = event_tab.getAttributeValues(event_att_index)
                    # create temporary dictionary
                    d = dict()
                    for key,value in itertools.izip(keys,values):
                        if key in d:
                            d[key].append(value)
                        else:
                            d[key]=[value]
                    # create metadata
                    temp_counter = 0
                    ban_list=[0,0,0,0]
                    for name, i in zip(aggr_names,range(len(aggr_names))):
                        full_name = event_tab.head[event_att_index]+'.'+name
                        # generate only requisite number of attributes
                        if self.created_total+temp_counter > self.created_limit:
                            break
                        # check for duplicity
                        elif full_name in self.database.customers.head:
                            ban_list[i]=1
                            continue
                        else:
                            # create names of new attributes in header of customer table
                            self.database.customers.head.append(full_name)
                            # set datatype
                            if i!=3:
                                self.database.customers.types.append('numeric')
                            else:
                                self.database.customers.types.append('nominal')
                            # increase attribute iterator
                            temp_counter+=1
                    # generate data
                    for customer in self.database.customers.body:
                        # help variables
                        key = customer[customer_key_index]
                        all_values = d[key] if key in d else []
                        def_values = [val for val in all_values if len(val)>0]
                        count = len(all_values)
                        unique_count = len(set(def_values))
                        best_count = collections.Counter(def_values).most_common(1)
                        if len(def_values)>0:
                            favourite_frequency = best_count[0][1]
                            favourite_item = best_count[0][0]
                        else:
                            favourite_frequency = 0
                            favourite_item = ''
                        # append data
                        if self.created_total+1 <= self.created_limit and ban_list[0]==0:
                            # count
                            customer.append(str(count))
                        if self.created_total+2 <= self.created_limit and ban_list[1]==0:
                            # unique count                     
                            customer.append(str(unique_count))
                        if self.created_total+3 <= self.created_limit and ban_list[2]==0:
                            # largest frequency
                            customer.append(str(favourite_frequency))
                        if self.created_total+3 <= self.created_limit and ban_list[3]==0:
                            # most frequent item
                            customer.append(str(favourite_item))
                    # increase attribute iterator
                    self.created_total+=temp_counter