"""
109.73866306  18.34957740 109.73973595  18.28129500
109.75055061  18.28357695 109.75226723  18.34680752
109.76634346  18.34908860 109.76548515  18.28145800
109.78265129  18.28259898 109.78900276  18.34387465

(109.73866306, 18.34957740) (109.73973595, 18.28129500)
(109.75055061, 18.28357695) (109.75226723, 18.34680752)
(109.76634346, 18.34908860) (109.76548515, 18.28145800)
(109.78265129, 18.28259898) (109.78900276, 18.34387465)
"""

gps_info = """
109.73866306  18.34957740 109.73973595  18.28129500
109.75055061  18.28357695 109.75226723  18.34680752
109.76634346  18.34908860 109.76548515  18.28145800
109.78265129  18.28259898 109.78900276  18.34387465
"""


trackers = []

def split_gps_line(lng_start, lat_start, lng_end, lat_end, nums):
    lng_start = float(lng_start)
    lng_end = float(lng_end)
    lat_start = float(lat_start)
    lat_end = float(lat_end)

    delta_lng    = lng_end - lng_start
    delta_lat    = lat_end - lat_start
    interval_lng = delta_lng / nums
    interval_lat = delta_lat / nums

    points = []
    for steps in range(0, nums):
        points.append(lng_start+ steps * interval_lng)
        points.append(lat_start + steps * interval_lat)

    return points

gps_list = gps_info.split()
for i in range(0, len(gps_list), 4):
    trackers += split_gps_line(gps_list[i], gps_list[i+1], gps_list[i+2], gps_list[i+3], 20)

f = open('tmp.txt', 'w')

i = 0
for item in trackers:
    f.write(str(item))
    f.write(', ')
    i += 1
    if(i %2 == 0):
        f.write('\n')

f.close()
print(trackers)
