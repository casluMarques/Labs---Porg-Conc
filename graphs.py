import matplotlib.pyplot as plt

threads = [1,2,4,8]

aceleração_500 = [0.694 , 1.35 , 2.36, 1.93 ]

aceleração_1000 = [0.7,	1.36,	2.663,	2.47]

aceleração_2000 = [0.78,	1.545,	2.97,	2.51]

eficiencia_500 = [0.1735, 0.3375,	0.594,	0.48575]

eficiencia_1000 = [0.175,	0.34,	0.66575,	0.6175]

eficiencia_2000 = [0.195,	0.38625,	0.7425,	0.6275]

plt.plot(threads, aceleração_500)
plt.plot(threads, aceleração_1000)
plt.plot(threads, aceleração_2000)
plt.plot(threads, eficiencia_500)
plt.plot(threads, eficiencia_1000)
plt.plot(threads, eficiencia_2000)
plt.show()
