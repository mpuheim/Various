from operator import itemgetter

class Delimiter:
    # constructor
    def __init__(self,database):
        self.database = database
        self.target = None
        self.safe_list = None
        
    # elimination of attributes with low correlation with the target attribute
    def eliminate(self,target,safe_list=[],goal=3):
        # set key name
        key = self.database.key
        # generate attribute list omitting attributes from the safe list
        if not all(isinstance(item, str) or isinstance(item, unicode) for item in safe_list):
            raise ValueError('all items within safe_list need to be string')
        attr_list = [attr for attr in self.database.customers.head if attr not in safe_list and attr!=key and attr!=target]
        # eliminate only if number of attributes is higher than goal
        if len(attr_list) > goal:
            # set target index
            target_index = self.database.customers.getAttributeColumn(target)
            # get correlations with target (ranking list)
            ranking_list = list()
            for attr in attr_list:
                attr_index = self.database.customers.getAttributeColumn(attr)
                ranking_list.append([attr,self.database.customers.correlation(attr_index,target_index)])
            # sort ranking list
            ranking_list.sort(key=itemgetter(1),reverse=True)
            # remove attributes with very low correlation
            best = ranking_list[0][1]
            threshold = best * 0.1
            skipped = 0
            for attr in list(ranking_list):
                if skipped>=goal:
                    if attr[1] < threshold:
                        attr_index = self.database.customers.getAttributeColumn(attr[0])
                        self.database.customers.removeAttribute(attr_index)
                        ranking_list.remove(attr)
                        attr_list.remove(attr[0])
                else:
                    skipped+=1
            # get correlations of attributes against each other (distance list)
            distance_list = list()
            attr_count = len(attr_list)
            for i in range(0,attr_count-1):
                for j in range(i+1,attr_count):
                    attr_i_index = self.database.customers.getAttributeColumn(attr_list[i])
                    attr_j_index = self.database.customers.getAttributeColumn(attr_list[j])
                    distance_list.append([i,j,self.database.customers.correlation(attr_i_index,attr_j_index)])
            # sort distance list
            distance_list.sort(key=itemgetter(2),reverse=True)
            # create support list and cluster list
            support_list = list()
            cluster_list = list()
            for i in range(0,attr_count):
                support_list.append(i)
                cluster_list.append([i])
            # clustering
            non_empty_clusters = attr_count
            for dist in distance_list:
                # stop with expected number of clusters
                if non_empty_clusters <= goal:
                    break
                # set cluster indices
                cl1 = support_list[dist[0]]
                cl2 = support_list[dist[1]]
                # continue if clusters have already merged
                if cl1 == cl2:
                    continue
                # set cluster references
                cluster_1 = cluster_list[cl1]
                cluster_2 = cluster_list[cl2]
                # move items from one cluster to another (merge clusters)
                for att in cluster_2:
                    cluster_1.append(att)
                    support_list[att] = cl1
                # empty one of the merged clusters
                cluster_2[:] = []
                # increase empty cluster count
                non_empty_clusters -= 1
            # remove empty clusters from cluster list
            cluster_list = [cluster for cluster in cluster_list if len(cluster)!=0]
            # create list of best attributes in each cluster (winner list)
            winner_list = [ranking_list[cl[0]][0] for cl in cluster_list]
            # remove attributes other than winners
            skipped = 0
            for attr in attr_list:
                if attr not in winner_list:
                    attr_index = self.database.customers.getAttributeColumn(attr)
                    self.database.customers.removeAttribute(attr_index)
                