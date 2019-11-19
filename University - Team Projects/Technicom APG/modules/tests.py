import scipy, scipy.stats, numpy, itertools

# Chi Square Test of independence (returns probability that attributes are dependant)
def chiSqTest(attr1,attr2):
    # consistency check
    if len(attr1)!=len(attr2):
        raise ValueError('atribute lists are not the same size')
    # build class list
    class1 = list(set(attr1)-set(['']))
    class2 = list(set(attr2)-set(['']))
    # build contingency table
    cont_table = numpy.zeros((len(class1),len(class2)),numpy.int)
    # fill contingency table
    for val1,val2 in itertools.izip(attr1,attr2):
        #print val1, val2
        if len(val1)>0 and len(val2)>0:
            i = class1.index(val1)
            j = class2.index(val2)
            cont_table[i,j]+=1
    # perform chi square test
    try:
        chi2, p, dof, exp_frequencies = scipy.stats.chi2_contingency(cont_table)
    except ValueError:
        # print 'chi_sq_test_failed'
        return 0
    # validity check for expected frequencies table
    total_count = float(len(class1)*len(class2))
    invalid_count = 0.0
    for val in numpy.nditer(exp_frequencies):
        # test should be used only if expected frequency in each cell is at least 5
        if val<5:
            invalid_count+=1;
    # if test is invalid (more than 20% cells are below 5), return 0
    if invalid_count/total_count>0.2:
        return 0
    # if test is valid, return inverted probability
    return 1-p
    
# Pearson correlation coeficient for numeric attributes
def pearsonTest(attr1,attr2):
    # consistency check
    if len(attr1)!=len(attr2):
        raise ValueError('atribute lists are not the same size')
    # create attribute arrays
    attr1_list = list()
    attr2_list = list()
    for val1,val2 in itertools.izip(attr1,attr2):
        if len(val1)!=0 and len(val2)!=0:
            attr1_list.append(float(val1))
            attr2_list.append(float(val2))
    attr1_array = numpy.array(attr1_list,numpy.float)
    attr2_array = numpy.array(attr2_list,numpy.float)
    # calculate correlation
    return abs(scipy.stats.pearsonr(attr1_array, attr2_array)[0])
    