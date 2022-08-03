import matplotlib.pyplot as plt
import numpy as np

f = open('check_airway.txt', 'r')
context = f.readlines()

line1_x = list(map(float, context[0].split(',')[1:-1][0::2]))
line1_y = list(map(float, context[0].split(',')[1:-1][1::2]))

line2_x = list(map(float, context[1].split(',')[1:-1][0::2]))
line2_y = list(map(float, context[1].split(',')[1:-1][1::2]))

line3_x = list(map(float, context[2].split(',')[1:-1][0::2]))
line3_y = list(map(float, context[2].split(',')[1:-1][1::2]))

line4_x = list(map(float, context[3].split(',')[1:-1][0::2]))
line4_y = list(map(float, context[3].split(',')[1:-1][1::2]))
line5_x = list(map(float, context[4].split(',')[1:-1][0::2]))
line5_y = list(map(float, context[4].split(',')[1:-1][1::2]))
line6_x = list(map(float, context[5].split(',')[1:-1][0::2]))
line6_y = list(map(float, context[5].split(',')[1:-1][1::2]))
line7_x = list(map(float, context[6].split(',')[1:-1][0::2]))
line7_y = list(map(float, context[6].split(',')[1:-1][1::2]))
line8_x = list(map(float, context[7].split(',')[1:-1][0::2]))
line8_y = list(map(float, context[7].split(',')[1:-1][1::2]))

longitude = []
latitude = []
for l in context:
    #  print(l.split(',')[1:-1][0::2])
    #  print(l.split(',')[1:-1][1::2])
    longitude += l.split(',')[1:-1][0::2]
    latitude += l.split(',')[1:-1][1::2]

rect = {}
rect['x1'] = float(min(longitude))
rect['x2'] = float(max(longitude))
rect['y1'] = float(min(latitude))
rect['y2'] = float(max(latitude))

step = 0.000204
x_step = 87
y_step = 58

x = [rect['x1'], rect['x2']]
y = [rect['y1'], rect['y2']]

points_x = []
points_y = []
for i in range(y_step):
    #  for i in range(1):
    for j in range(x_step):
        x = rect['x1'] + step * j
        y = rect['y1'] + step * i
        points_x.append(x)
        points_y.append(y)
        if i % 4 == 0 and j % 4 == 0:
            #  plt.text(x, y, str(j) + ',' + str(i), fontsize=8)
            pass
            #  if i == 18 and j == 4:
            #  print(x, y)

#  plt.scatter(x, y, c='r', marker='o')
#  plt.scatter(points_x, points_y, c='b')
plt.plot(line1_x, line1_y, c='r')
plt.plot(line2_x, line2_y, c='r')
plt.plot(line3_x, line3_y, c='r')
plt.plot(line4_x, line4_y, c='r')
plt.plot(line5_x, line5_y, c='r')
plt.plot(line6_x, line6_y, c='r')
plt.plot(line7_x, line7_y, c='r')
plt.plot(line8_x, line8_y, c='r')

origin_x = 109.44099563543718
origin_y = 19.556514394679166

endpoints_x = [109.44149163058223, 109.44087210163222]
endpoints_y = [19.55586709270411, 19.557417422760206]

#  endpoints_x = [109.44149178040347, 109.44087227736468]
#  endpoints_y = [19.555867146190508, 19.557417485519544]

plt.scatter(origin_x, origin_y, c='g', marker='o')
plt.plot(endpoints_x, endpoints_y, c='y', marker='o')

plt.xlim((109.44, 109.45))
plt.ylim((19.552, 19.562))

plt.grid(True)
plt.show()
