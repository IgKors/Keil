// ??????? ???????? ? ????????? ????????????? ? ??????? ???? ??????, ???????? ???????? ? EEPROM

int8_t Npreset  = 35-1;	//?????????? ????????


/*?????? ????????, 
??????? ???????? - ????? ??? ?????? ???????, ?????? ???????? - ?????????? ?????? ? ???????*/

/*
******
Cancer
******

 Rife Freq's
 666, 690, 727

 Bare & DT Freq's
 2489

 Turf Freq's
 ---

 Garvey Freq's
 ---

 Other Freq's
 787, 880, 1550, 2008, 2050, 2127


see programs below full frequency sets.
All others below are secondary but may
provide useful additional frequencies
depending on type. Also add ones
determined from scans.

**************************
Cancer (general program 1)
**************************
rotate between these programs, end with 304

 Rife Freq's
 ---

 Bare & DT Freq's
 120, 304, 464, 664, 728, 800, 880, 2008, 2120, 2128, 2176, 2184, 2720, 3176, 10000

 Turf Freq's
 120, 304, 464, 524, 666, 728, 784, 800, 854, 880, 2008, 2048, 2128, 2184, 2452, 2720, 3176, 5000, 6064, 10000

 Garvey Freq's
 ---

 Other Freq's
 ---

********************************
Cancer, carcinoma (general scan)
********************************
any cancerous tumor starting with cells
covering body and organ sufaces that
then invade both local and distant areas

 Rife Freq's
 2127 

 Bare & DT Freq's
 2128, 2150+-50, 2184

 Turf Freq's
 304, 690, 728, 2008, 2104, 2112, 2120, 2128, 2136, 2144, 2152, 2160, 2168, 2176, 2184, 2192, 2200, 2217, 5000, 9999

 Garvey Freq's
 ---

 Other Freq's
 666, 727, 787, 880, 2127, 7130 
*/

// ?????????????? ??? ????
// 47, 2182, 2219,	832,  2084,	2127, 2160,	2452, 2876  (Adenocarcinoma Z-App)

// ????    ??????????????  ??????????
// Cancer, adenocarcinoma, esophageal
// ??????? ? ??????? ? ?????? ???????, ???????????? - 35.75 ? 47 Hz ??????? ??????
// 47, 832,  2084,  2127, 2160, 2182, 2219, 2452, 2876 	(Turf Freq's -> frequency_table_master_list.pdf)

// ??????? ??????? ??????? ? ?????? ?? ???? ?? ????? (Rife Freq's)  - 666, 690, 727

// 666, 690, 727, 	832,  	2084,  	2127,	2160, 	2182, 	2219, 	2452, 	2876
// Z-App			832		2084	2127	2160	2182	2219	2452	2876
//

// TEST
//uint32_t PR000[]  = {	3,4,				11100,		22200,		33300,			44400	};																	


// N029 Adenovirus
// 333/523/786/768
uint32_t PR000[]  = {	180,4,			33300, 		52300, 		78600, 		76800
																	};

				

// N002 Bronchitis
// 880/727.5/9.39/9.35/1234/3672/7344/743/20/72/333/452/464/683

uint32_t PR001[]  = {	60,14,			939, 			935, 			88000,	 	2000, 		7200,
																	33300,		46400,		45200,		68300,		72750,				
																	74300,		123400, 	367200,		734400	 		
																	};

// N003 Bronchitis
// 727.5/880
uint32_t PR002[]  = {	300,2,			72750, 		88000};	

// N004 Bronchial Asthma 
// 2720/2170/1800/1600/1500/1283/1233/880/787/727.5/0.5/522/146/125/95/72/444/20

uint32_t PR003[]  = {	60,18,			50,				2000,			4400,			7200,		9500,
																	12500,		14600,		52200,		72750,		78700, 
																	88000,		272000, 	217000, 	180000,		160000,		
																	150000, 	128300, 	123300
																	};								


// N005 Pneumonia/bronchial
// 550/802/880/787/776/727.5/452/1474/578
uint32_t PR004[]  = {	120,9,			55000, 		80200, 		88000, 		78700, 		77600, 
																	72750, 		45200, 		147400, 	57800
																	};			

// N006 Cystitis/chronic  ??????, ???????????
// 246/1550/880/802/787/727.5/465/20
uint32_t PR005[]  = {	120,8,			24600, 		155000, 	88000, 		80200, 		78700, 
																	72750, 		46500, 		2000
																	};


// N007 ??????? - Gastritis and flatus
// 880/832/802/787/727.5/676/422/20
uint32_t PR006[]  = {	180,6,			88000, 		83200, 		80200, 		78700, 		72750, 
																	67600, 		2000
																	};


// N008 ?????????????? ???????? - Ulcer/gastric
// 676 
uint32_t PR007[]  = {	600,1,			67600};				

// N009 ???? ?????????????????? ????? - Duodenal Ulcer:
// 676/727.5/750/880/10000
uint32_t PR008[]  = {	180,5,			67600, 		72750, 		75000, 		88000, 		10000
												};							

// N010
// ????? ? ??????
// Kidney Stones:
// ???? - ????????
// Kidney Calculi: 
// 30.5/787/444/1865/660/690/727.5/880/1552/3000/6000/10000
uint32_t PR009[]  = { 120,12,			1552,			3050, 		44400,		78700, 		66000,	 		 	 
																	69000,		72750, 		88000, 		186500,		300000,
																	600000,		1000000
																	};

// N011 ??????? ?????
// Kidney tonic / GENERAL (????????? ?????): 
uint32_t PR010[]  = {	120,9,			800, 			2000,			24800, 		44000, 		88000, 		 
																	80000, 		500000, 	300000,		1000000
																	};

// N012 ?????????? ????????? ??????????? ?????
// kidney cyst pyelonephritis
uint32_t PR011[]  = {	600,1,			138500};

// N013
// ?????? ???????? ???????????????
// Kidney Failure, Acute: 
// ??????????? ???????? ???????????????
// Kidney Failure, Chronic:
// ?????????? ?????? ?????
// Kidney Tubular Necrosis: 
// 9.2, 10, 20, 40, 72, 95, 125, 146, 250, 440, 444, 600, 625, 650, 802, 880, 1500, 1550, 1600, 1865

uint32_t PR012[]  = {	60,21,			920, 			1000, 		2000, 		4000, 		7200, 
																	9500, 		12500, 		14600, 		25000, 		14600, 
																	25000,		44000,		44400,		60000,		62500,
																	65000,		80200,		88000,		150000,		160000,
																	186500
																	};

// N014 Adenoids
// 1550/802/880/787/776/727.5/444/20/428/660/2720/2170
uint32_t PR013[]  = {	120,12, 		1550,			2000,			42800,		44400,		66000,
																	72750,		77600,		78700,		80200, 		88000,		
																	217000,		272000		
																	};

// N015 Angina (quinsy in swat)
// 787/776/727.5/465/428/660
uint32_t PR014[]  = {	180,6,			42800,		46500,		66000,		72750,		77600,
																	78700
																	};

// N016 Angina Pectoris
// 3/230/2720/2170/1800/1600/1500/880/832/787/776/727.5/465/444/1865/125/95/72/20/660/7.83
 uint32_t PR015[]  = {	60,21,		300, 			783,			2000,			7200,			9500,
																	23000,		44400,		46500,		66000,		72750,
																	12500,		77600,		78700,		83200,		88000,
																	150000, 	160000,		180000, 	186500,		217000,
																	272000 	
																	};

// N017 Arthritis
// 120/727.5/787/880/1550/802/1664/80/60/40/30/25/26/20/10/5000/10000/7.69/3/1.2/28/1.5
 uint32_t PR016[]  = {	60,22,		120,			150,			769,			1000,			2000,
																	2500,			3000,			2600,			2800,			3000,
																	4000,			6000,			8000,			12000,		72750,
																	78700, 		80200,		88000, 		155000, 	166400,
																	500000,		1000000			
												};

//N018 Prostatis
// 100/410/522/146/2720/2489/2170/2127/2008//1550/802/787//776/727.5/690/666/465/125/96/72/20/444/522//9.1
 uint32_t PR017[]  = {	60,22,		910,			960,			2000,			7200,			10000,
																	12500, 		14600,		41000, 		44400,		46500,
																	52200, 		66600, 		69000,		72750,		77600,
																	78700,		80200,		155000,		217000,		200800, 
																	248900,		272000						
																	};

// N019 Tuberculinum
// 332/522/664/731/737/748/1085/1099/1700/761
 uint32_t PR018[]  = {	120,9,		33200, 		52200, 		64400, 		73700, 		74800, 
																	76100,		108500,		109900,		170000
												};

//N020 Tuberculosis also see Tuberculinum
// 20/740/1840/720/1552/1600/216/802/784/1500/727.5/690/666
 uint32_t PR019[]  = {	120,13,		2000, 		21600,		66600,		69000,		72000,		
																	72750,		74000, 		78400,		80200, 	 	150000,	 
																	155200,		160000,		184000						
																	};

// N021 Tuberculosis/virus
// 2565
 uint32_t PR020[]  = {	600,1,			256500};

// N021 Allergies 
// 10000/880/787/727.5/3/330/5000
 uint32_t PR021[]  = {	180,8,			300,		500,			33000,		72750,		78700,
																		8000,		500000,		1000000		
																		};
													
//PR022 Diabetes (1)
// 4200/2128/1865/1850/1550/787/465/444/125/95/72/48/302
 uint32_t PR022[]  = {	120,13,			4800,		7200,			9500,			12500,		30200,
																		44400,	46500,		78700,		155000,		185000,
																		186500,	212888,		420000		
																		};

// PR023 Candida III
// 20/60/100/95/125/152/225/240/427/442/600/625/650/688/751/880/1146/8146/19217.81/956.8
 uint32_t PR023[]  = {	60,20, 			2000,		6000, 		9500,			10000,		12500,
																		15200,	22500,		24000,		42700,		44200,      
																		60000,	62500,		65000,		68800,		75100,		
																		88000,	95680,		114600,		814600,		1921781	
																		};

// PR024 Bacteria Helicobacter pylori
// 660/690/727.5/676/880/2167/2950
 uint32_t PR024[]  = {	180,7, 			66000,		69000,		67600,		72750,		88000,
																		216700,		295000	
																		};

// PR025 Hepatitis - general
// 28/1550/1351/880/802/727.5/224/317/329/477/922
 uint32_t PR025[]  = {	120,11,			2800,			22400,		31700,		32900,		47700,
																		72750,		80200,		88000,		92200,		135100, 								
																		155000
																		};

// PR026 Hepatitis - general secondary
// 284/458/477/534/788/922/9670/768/777/1041
 uint32_t PR026[]  = {	120,10,			1041,			28400,		45800,		47700, 		53400,		
																		78800,		76800,		77700,		92200,		967000,		
												};

// PR027 Staphylococcus general
// 453/550/674/728/1109/7160/634/639/1089
 uint32_t PR027[]  = {	120,9,			45300,		55000,		63400,		63900,		67400,		
																		716000,		72800,		108900, 	110900
												};

// PR028 Tachycardia
// 1,2
 uint32_t PR028[]  = {	600,1,			120};

// N001 ??????????????
uint32_t PR029[]  = {	180,11,				66600, 	69000, 	72700, 	83200,  208400,  
																		212700, 216000, 218200, 221900, 245200, 
																		287600
																		};


// N001 ??????
// 597.53  / 796.71   / 1062.28  
// 597530  / 796710   / 1062280
static uint32_t PR030[] = {	0,3,		597530,		796710, 	1062280};				

// N001 ?????????????
// 1024.337
// 341.4457  + 1024.337 = 1365.783 
// 1365.783  + 455.261  = 1821.044
// 1024337   / 1365783  / 1821044
static uint32_t PR031[] = {	0,3,		1024337, 	1365783, 	1821044};

// N002 ??????
// 1344.553+
// 1344.553  + 448.1843  = 1792.737
// 1792.737  + 597.5791  = 2390.316
// 1344553   / 1792737   / 2390316
static uint32_t PR032[] = {	0,3,		1344553,	1792737, 	2390316};

// N003 ??.?????
// 1510.627
// 1510.627  + 503.5423  = 2014.169
// 2014.1693 + 671.38977 = 2685.559
// 1510627   / 2014169   / 2685559
static uint32_t PR033[] = {	0,3,		1510627,	2014169, 	2685559};


// N004 ??.?????
// 1510.610
// 1510.610  + 503.53666 = 2014.147
// 2014.1467 + 671.38222 = 2685.529
// 1510610   / 2014147 / 2685529
static uint32_t PR034[] = {	0,3,		1510610,	2014147, 	2685529};			


// ?????? ?????? ?? ???????
uint32_t *PR[] = {
PR000, 
PR001,
PR002,
PR003,
PR004,
PR005,
PR006,
PR007,
PR008,
PR009,
PR010,
PR011,
PR012,
PR013,
PR014,
PR015,
PR016,
PR017,
PR018,
PR019,
PR020,
PR021,
PR022,
PR023,
PR024,
PR025,
PR026,
PR027,
PR028,
PR029,
PR030,
PR031,
PR032,
PR033,
PR034
};

// ???????? ????????
char* PN[] = {


//"N001 Adenovirus",
//"?001 ??????????"
  "??????????",

//"N002 Bronchitis",
//"?002 ???????",
  "??????? I",

//"N003 Bronchitis",
  "??????? II",


//"N004 Bron/Asthm",
//"N004 ????/?????",
  "???????????? ?????",

//"N005 Pneumonia",
//"?005 ?????????",
  "?????????",

//"N006 Cystitis",
//"N006 ??????",
  "??????",

//"N007 Gastritis",
//"?007 ???????",
  "???????",

//"N008 Bron/Cancer",
//"?008 ???????????",
  "?????????????",

//"N009 Duodenal Ulcer"
//"?009 ???????? ??????? ? ????????"
  "???????? ????????",

//"N010 ???.?????",  
  "????? ? ??????",

//"N011 ???.?????.",  
  "?????????? ?????",

//"N012 ???/?????",  
  "????? ?????",

//"N013 ???/??????",  
  "???????? ??????.",

//"N014 Adenoids"
  "????????",

//N015 Angina (quinsy in swat)
  "?????? I",

//N016 Angina Pectoris
  "?????? Pectoris",

//N017 Arthritis
  "??????",

//N018 Prostatis
  "?????????",

//N019 Tuberculinum
  "?????????? 1",

//N020 Tuberculosis also see Tuberculinum
  "?????????? 2",

//N021 Tuberculosis/virus
  "???????? ??????.",

//N022 Allergies (???????)  
  "???????",

//N023 Diabetes
  "??????",

//N024 Candida III	  
  "???????",
  
//N025 Bacteria Helicobacter pylori  
  "???????????",

//N026 Hepatitis - general  
  "??????? I",		 

//N027 Hepatitis - general secondary	
  "??????? II",

//N028 Staphylococcus general
  "???????????",

//N029 Tachycardia
  "??????????",

// N030 Adenocarcinoma
  "??????????????",
	
// ??????
  "??????",

// ?????????????
  "?????????????",

// ??????
  "???????",

// ??.?????
  "????? ?????",

// ??.?????
  "?????? ?????"
 };
