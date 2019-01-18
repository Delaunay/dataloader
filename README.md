

Initial Test

* Single Threaded
* 2 Hard Drives in RAID

* ▶ ./bin/io_benchmark --data /media/setepenre/UserData/tmp/fake -n 2048 
    - 32842 images found
    - 100 classes found
    - took 2.22461s to initialize
* Read 2048 images in 20.6777 sec
    - 99.0439 file/sec
    - 59.5327 Mo/sec
    - 615.9674 Ko
* Decoded 2048 images in 26.3394 sec
    - 77.7543 file/sec
    - 14.5789 Mo/sec
* Scaling 2048 images in 3.7789 sec
    - 541.9519 file/sec
    - 12.7020 Mo/sec
* Total 2048 images in 47.0171 sec
    - 43.5586 file/sec
    - Overhead 0.0198 sec 
    - Compression Ratio After downscale  0.3117
    - Compression Ratio before downscale -1.662


# Thread Pool

on a 12 cores processor.

* Thread Count = 1


     Thread Pool Report
       ID       WORK       IDLE   (%)  TASKS     WORK/TASK
        0   148.9534     0.3578  99.76   200     0.7448
    Total   148.9534     0.3578  99.76   200     0.7448

                     TIME   (%)
    Empty Queue      0.36   0.24
    Full  Queue   3804.10 2547.75
       All Time    149.31 100.00

        Arrival Rate     18.92
      Departure Rate     28.47


* Thread Count = 2


    std::size_t vector_size = 100000000;
    std::size_t queue_size = 100;
    std::size_t worker_size = 2;
    std::size_t request_count = 200;

     Thread Pool Report
       ID       WORK       IDLE   (%)  TASKS     WORK/TASK
        0    76.2848     0.5450  99.29   100     0.7628
        1    76.4483     0.3841  99.50   100     0.7645
    Total   152.7332     0.9292  99.40   200     0.7637

                     TIME   (%)
    Empty Queue      0.37   0.48
    Full  Queue   1910.11 2486.06
       All Time     76.83 100.00

        Arrival Rate      9.62
      Departure Rate     30.84


* Thread Count = 4


    std::size_t vector_size = 100000000;
    std::size_t queue_size = 100;
    std::size_t worker_size = 4;
    std::size_t request_count = 200;

         Thread Pool Report
       ID       WORK       IDLE   (%)  TASKS     WORK/TASK
        0    38.8584     0.4447  98.87    50     0.7772
        1    38.8188     0.4838  98.77    50     0.7764
        2    38.7191     0.5762  98.53    50     0.7744
        3    38.6531     0.6467  98.35    50     0.7731
    Total   155.0494     2.1514  98.63   200     0.7752

                     TIME   (%)
    Empty Queue      0.43   1.09
    Full  Queue    883.09 2246.85
       All Time     39.30 100.00

        Arrival Rate      5.12
      Departure Rate     13.43


* Thread Count = 6


     Thread Pool Report
       ID       WORK       IDLE   (%)  TASKS     WORK/TASK
        0    25.8212     1.0470  96.10    33     0.7825
        1    25.8546     1.0150  96.22    33     0.7835
        2    26.4608     0.4096  98.48    34     0.7783
        3    25.8022     1.0649  96.04    33     0.7819
        4    26.0243     0.8451  96.85    33     0.7886
        5    26.4194     0.4501  98.32    34     0.7770
    Total   156.3825     4.8316  97.00   200     0.7819

                     TIME   (%)
    Empty Queue      0.41   1.53
    Full  Queue    586.06 2180.89
       All Time     26.87 100.00

        Arrival Rate      3.54
      Departure Rate     13.16


* Thread Count = 8


    std::size_t vector_size = 100000000;
    std::size_t queue_size = 100;
    std::size_t worker_size = 8;
    std::size_t request_count = 200;

       ID       WORK       IDLE   (%)  TASKS     WORK/TASK
        0    25.0838     1.1692  95.55    26     0.9648
        1    25.8188     0.4343  98.35    24     1.0758
        2    25.1089     1.1377  95.67    25     1.0044
        3    25.2954     0.9478  96.39    23     1.0998
        4    25.6694     0.5752  97.81    26     0.9873
        5    25.3231     0.9286  96.46    27     0.9379
        6    25.5590     0.6871  97.38    26     0.9830
        7    25.0545     1.1886  95.47    23     1.0893
    Total   202.9128     7.0684  96.63   200     1.0146

                     TIME   (%)
    Empty Queue      0.41   1.58
    Full  Queue    596.63 2272.58
       All Time     26.25 100.00

        Arrival Rate      3.40
      Departure Rate     13.41


* Thread Count = 12

     Thread Pool Report
       ID       WORK       IDLE   (%)  TASKS     WORK/TASK
        0    23.3366     1.1384  95.35    16     1.4585
        1    24.1295     0.3467  98.58    17     1.4194
        2    24.0249     0.4499  98.16    17     1.4132
        3    23.6484     0.8265  96.62    16     1.4780
        4    24.0752     0.3993  98.37    17     1.4162
        5    24.0456     0.4289  98.25    17     1.4144
        6    23.9277     0.5472  97.76    17     1.4075
        7    24.1644     0.3113  98.73    17     1.4214
        8    23.0629     1.4119  94.23    16     1.4414
        9    23.7572     0.7182  97.07    17     1.3975
       10    24.1356     0.3402  98.61    17     1.4197
       11    23.0817     1.3925  94.31    16     1.4426
    Total   285.3898     8.3110  97.17   200     1.4269

                     TIME   (%)
    Empty Queue      0.30   1.24
    Full  Queue    339.55 1387.23
       All Time     24.48 100.00

        Arrival Rate      2.98
      Departure Rate     13.59


* Thread Count = 16

     Thread Pool Report
       ID       WORK       IDLE   (%)  TASKS     WORK/TASK
        0    23.6290     1.1470  95.37    12     1.9691
        1    24.2335     0.5447  97.80    13     1.8641
        2    24.0533     0.7225  97.08    13     1.8503
        3    24.0827     0.6939  97.20    13     1.8525
        4    24.3430     0.4351  98.24    13     1.8725
        5    23.4459     1.3312  94.63    12     1.9538
        6    23.4349     1.3419  94.58    12     1.9529
        7    24.3463     0.4320  98.26    13     1.8728
        8    23.4334     1.3442  94.57    12     1.9528
        9    23.4087     1.3686  94.48    12     1.9507
       10    23.7325     1.0431  95.79    13     1.8256
       11    23.4946     1.2825  94.82    12     1.9579
       12    24.1663     0.6121  97.53    12     2.0139
       13    23.6089     1.1692  95.28    12     1.9674
       14    24.4008     0.3774  98.48    13     1.8770
       15    24.0737     0.7034  97.16    13     1.8518
    Total   381.8876    14.5489  96.33   200     1.9094

                     TIME   (%)
    Empty Queue      0.38   1.53
    Full  Queue    477.66 1927.61
       All Time     24.78 100.00

        Arrival Rate      2.94
      Departure Rate     14.30


* Summary


	    Timings (s)				Speed up (x)
    Cores	Threads	Runtime	Time/Task		Threads	Runtime	Time/Task
    12	1	149.31	0.7448		0.50	0.515	1.025
    12	2	76.83	0.7637		1.00	1.000	1.000
    12	4	39.3	0.7752		2.00	1.955	0.985
    12	6	26.87	0.7819		3.00	2.859	0.977
    12	8	26.25	1.0146		4.00	2.927	0.753
    12	10	25.33	1.2393		5.00	3.033	0.616
    12	12	24.48	1.4269		6.00	3.138	0.535
    12	14	24.87	1.6754		7.00	3.089	0.456
    12	16	24.78	1.9094		8.00	3.100	0.400

* Dataloader


* Thread Count = 1 (batch=32)
    Total 1024 images in 37.0658 sec
     - 27.6266 file/sec / thread
     - 27.3636 file/sec overall
     - Single Thread Time 37.4220 sec 
     - Task Time 37.0658
     - Overhead 0.3563 sec 
     - Compression Ratio before scaling 4.9874
     - Compression Ratio after scaling 0.2387

* Thread Count = 2 (batch=32)
    Total 1024 images in 39.4663 sec
     - 25.9462 file/sec / thread
     - 50.6838 file/sec overall
     - Single Thread Time 20.2037 sec 
     - Task Time 39.4663
     - Overhead -19.2626 sec 
     - Compression Ratio before scaling 4.9873
     - Compression Ratio after scaling 0.2386

* Thread Count = 4 (batch=32)
    Total 1024 images in 54.2277 sec
     - 18.8833 file/sec / thread
     - 71.5511 file/sec overall
     - Single Thread Time 14.3114 sec 
     - Task Time 54.2277
     - Overhead -39.9162 sec 
     - Compression Ratio before scaling 4.9922
     - Compression Ratio after scaling 0.2389

* Thread Count = 6 (batch=32)
    Total 1024 images in 76.1220 sec (combined thread) 
     - 13.4521 file/sec / thread
     - 73.9286 file/sec overall
     - Single Thread Time 13.8512 sec 
     - Overhead -62.2708 sec 
     - Compression Ratio before scaling 5.0867
     - Compression Ratio after scaling 0.2434



