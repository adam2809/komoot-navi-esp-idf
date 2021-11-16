EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:LED D?
U 1 1 617DC824
P 7950 3800
F 0 "D?" V 7989 3683 50  0000 R CNN
F 1 "LED" V 7898 3683 50  0000 R CNN
F 2 "" H 7950 3800 50  0001 C CNN
F 3 "~" H 7950 3800 50  0001 C CNN
	1    7950 3800
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D?
U 1 1 617DCF72
P 9550 3800
F 0 "D?" V 9589 3683 50  0000 R CNN
F 1 "LED" V 9498 3683 50  0000 R CNN
F 2 "" H 9550 3800 50  0001 C CNN
F 3 "~" H 9550 3800 50  0001 C CNN
	1    9550 3800
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D?
U 1 1 617DD526
P 8700 3800
F 0 "D?" V 8739 3683 50  0000 R CNN
F 1 "LED" V 8648 3683 50  0000 R CNN
F 2 "" H 8700 3800 50  0001 C CNN
F 3 "~" H 8700 3800 50  0001 C CNN
	1    8700 3800
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D?
U 1 1 617DE995
P 10400 3800
F 0 "D?" V 10439 3683 50  0000 R CNN
F 1 "LED" V 10348 3683 50  0000 R CNN
F 2 "" H 10400 3800 50  0001 C CNN
F 3 "~" H 10400 3800 50  0001 C CNN
	1    10400 3800
	0    -1   -1   0   
$EndComp
$Comp
L Regulator_Switching:LM2596S-3.3 U?
U 1 1 617E2E7E
P 4900 3000
F 0 "U?" H 4900 3367 50  0000 C CNN
F 1 "LM2596S-3.3" H 4900 3250 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TO-263-5_TabPin3" H 4950 2750 50  0001 L CIN
F 3 "http://www.ti.com/lit/ds/symlink/lm2596.pdf" H 4900 3000 50  0001 C CNN
	1    4900 3000
	1    0    0    -1  
$EndComp
$Comp
L RF_Module:ESP32-WROOM-32 U?
U 1 1 617E48CB
P 6650 4100
F 0 "U?" H 6650 5681 50  0000 C CNN
F 1 "ESP32-WROOM-32" H 6650 2900 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 6650 2600 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf" H 6350 4150 50  0001 C CNN
	1    6650 4100
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery BT?
U 1 1 617DF23F
P 4100 2900
F 0 "BT?" V 3855 2900 50  0000 C CNN
F 1 "2S_18650" V 3946 2900 50  0000 C CNN
F 2 "" V 4100 2960 50  0001 C CNN
F 3 "~" V 4100 2960 50  0001 C CNN
	1    4100 2900
	0    1    1    0   
$EndComp
Wire Wire Line
	4300 2900 4400 2900
Wire Wire Line
	3900 2900 3900 3300
Wire Wire Line
	3900 3300 4900 3300
Wire Wire Line
	4900 3300 4900 5500
Wire Wire Line
	4900 5500 6650 5500
Connection ~ 4900 3300
Wire Wire Line
	5400 3100 5700 3100
Wire Wire Line
	5700 3100 5700 2700
Wire Wire Line
	5700 2700 6650 2700
$Comp
L Transistor_BJT:2N3904 Q?
U 1 1 6183C99F
P 7850 3050
F 0 "Q?" H 8040 3096 50  0000 L CNN
F 1 "2N3904" H 8040 2900 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 8050 2975 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N3904.pdf" H 7850 3050 50  0001 L CNN
	1    7850 3050
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N3904 Q?
U 1 1 6183D3C5
P 8600 3050
F 0 "Q?" H 8790 3096 50  0000 L CNN
F 1 "2N3904" H 8790 3005 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 8800 2975 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N3904.pdf" H 8600 3050 50  0001 L CNN
	1    8600 3050
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N3904 Q?
U 1 1 61840984
P 9450 3050
F 0 "Q?" H 9640 3096 50  0000 L CNN
F 1 "2N3904" H 9640 3005 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 9650 2975 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N3904.pdf" H 9450 3050 50  0001 L CNN
	1    9450 3050
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N3904 Q?
U 1 1 61840E5D
P 10300 3050
F 0 "Q?" H 10490 3096 50  0000 L CNN
F 1 "2N3904" H 10490 3005 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 10500 2975 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N3904.pdf" H 10300 3050 50  0001 L CNN
	1    10300 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 2700 7950 2700
Wire Wire Line
	10400 2700 10400 2850
Connection ~ 6650 2700
Wire Wire Line
	9550 2700 9550 2850
Connection ~ 9550 2700
Wire Wire Line
	9550 2700 10400 2700
Wire Wire Line
	8700 2700 8700 2850
Connection ~ 8700 2700
Wire Wire Line
	8700 2700 9550 2700
Wire Wire Line
	7950 2700 7950 2850
Connection ~ 7950 2700
Wire Wire Line
	7950 2700 8700 2700
Wire Wire Line
	7950 3250 7950 3650
Wire Wire Line
	8700 3250 8700 3650
Wire Wire Line
	9550 3250 9550 3650
Wire Wire Line
	10400 3650 10400 3250
Wire Wire Line
	7950 3950 7950 5500
Wire Wire Line
	6650 5500 7950 5500
Connection ~ 6650 5500
Wire Wire Line
	8700 3950 8700 5500
Wire Wire Line
	8700 5500 7950 5500
Connection ~ 7950 5500
Wire Wire Line
	9550 3950 9550 5500
Wire Wire Line
	9550 5500 8700 5500
Connection ~ 8700 5500
Wire Wire Line
	10400 5500 9550 5500
Wire Wire Line
	10400 3950 10400 5500
Connection ~ 9550 5500
Wire Wire Line
	7250 4800 7650 4800
Wire Wire Line
	7650 4800 7650 3050
Wire Wire Line
	7250 4900 8400 4900
Wire Wire Line
	8400 4900 8400 3050
Wire Wire Line
	7250 5000 9250 5000
Wire Wire Line
	9250 5000 9250 3050
Wire Wire Line
	7250 5200 10100 5200
Wire Wire Line
	10100 5200 10100 3050
$EndSCHEMATC
