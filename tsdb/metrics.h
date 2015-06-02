//struct metric* metric_create(char* name) {
//	int len;
//	struct metric* metric;
//
//	metric = (struct metric*)malloc(sizeof(struct metric*));
//
//	// Copy name
//	len = strlen(name) + 1;
//	metric->name = (char*)malloc(len);
//	strcpy_s(metric->name, len, name);
//
//	metric->pointcount = 0;
//	metric->points = (struct point**)malloc(sizeof(struct point*) * 10);
//	metric->capacity = 10;
//
//	// TODO: Do we need to set all points to NULL?
//
//	return metric;
//}
//
//void metric_point_add(struct metric* metric, int timestamp, int count) {
//	// Check if we need to grow point array
//	struct point* point = (struct point*)malloc(sizeof(struct point*));
//	point->timestamp = timestamp;
//	point->count = count;
//	metric->points[metric->pointcount++] = point;
//}
//
//struct point* metric_point_find(struct metric* metric, int timestamp) {
//	int i;
//	struct point* point;
//
//	// TODO: Binary search
//	for(i = 0; i < metric->pointcount; i++) {
//		point = metric->points[i];
//		if(point == NULL) {
//			return NULL;
//		}
//
//		if(point->timestamp == timestamp) {
//			return point;
//		}
//	}
//
//	return NULL;
//}