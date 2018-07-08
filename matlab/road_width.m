% x=0:0.01:1
% y=asin(x)
% plot(x,y)
% y=0:10:110
% x=[0 1 2.5 5.5 8 11 14 16 19 23 25 28.5]
% plot(x,y)

% f=abs(-0.03008*a^2+4.515*a+4.768)

% t=0:0.01:20
% y=atan(t)*180/3.1415926
% plot(t,y)

% tan(28.5/180*3.1415926)

% s=0:1:250
% a=atan(s/100)*180/3.1415926/2.4
% f=-0.03008*a.^2+4.515*a+4.768
% plot(s,f)
% fprintf('%d, ',round(f))

% Zhejiang A&F University Anthem//浙江农林大学校歌 on Matlab 

% fs = 44100;                           % sample rate 采样率
% dt = 1/fs; 
% T16 = 0.125;                          %十六分之一拍的时间
% t16 = [0:dt:T16]; 
% [temp k] = size(t16); 
% 
% t1 = linspace(0,16*T16,16*k);
% t2 = linspace(0,8*T16,8*k);
% t4 = linspace(0,4*T16,4*k); 
% t8 = linspace(0,2*T16,2*k); 
% 
% [temp a] = size(t1);
% [temp b] = size(t2);
% [temp i] = size(t4); 
% [temp j] = size(t8); 
% 
% % Modification functions  包络函数
% mod1=(t1.^4).*exp(-15*(t1.^0.5)); 
% mod1=mod1*(1/max(mod1));         % 调整，使范围控制在0-1
% mod2=(t2.^4).*exp(-20*(t2.^0.5)); 
% mod2=mod2*(1/max(mod2)); 
% mod4=(t4.^4).*exp(-30*(t4.^0.5)); 
% mod4=mod4*(1/max(mod4)); 
% mod8=(t8.^4).*exp(-50*(t8.^0.5)); 
% mod8=mod8*(1/max(mod8)); 
% mod16=(t16.^4).*exp(-90*(t16.^0.5)); 
% mod16=mod16*(1/max(mod16));
% 
% f0=440;                           % reference frequency 标准音 基准频率
% 
% ScaleTable = [-21 -19 -17 -16 -14 -12 -10 ...
% 	-9 -7 -5 -4 -2 0 2 ...
% 	3 5 7 8 10 12 14];
% 
% % 4/4 notes 全音符
% do0o = mod1.* sin(2*pi*f0*2^(ScaleTable(1)/12)*t1);          %低音
% re0o = mod1.* sin(2*pi*f0*2^(ScaleTable(2)/12)*t1); 
% mi0o = mod1.* sin(2*pi*f0*2^(ScaleTable(3)/12)*t1); 
% fa0o = mod1.* sin(2*pi*f0*2^(ScaleTable(4)/12)*t1); 
% so0o = mod1.* sin(2*pi*f0*2^(ScaleTable(5)/12)*t1); 
% la0o = mod1.* sin(2*pi*f0*2^(ScaleTable(6)/12)*t1); 
% ti0o = mod1.* sin(2*pi*f0*2^(ScaleTable(7)/12)*t1); 
% do1o = mod1.* sin(2*pi*f0*2^(ScaleTable(8)/12)*t1);          %标准音
% re1o = mod1.* sin(2*pi*f0*2^(ScaleTable(9)/12)*t1); 
% mi1o = mod1.* sin(2*pi*f0*2^(ScaleTable(10)/12)*t1); 
% fa1o = mod1.* sin(2*pi*f0*2^(ScaleTable(11)/12)*t1); 
% so1o = mod1.* sin(2*pi*f0*2^(ScaleTable(12)/12)*t1); 
% la1o = mod1.* sin(2*pi*f0*2^(ScaleTable(13)/12)*t1); 
% ti1o = mod1.* sin(2*pi*f0*2^(ScaleTable(14)/12)*t1); 
% do2o = mod1.* sin(2*pi*f0*2^(ScaleTable(15)/12)*t1);         %高音
% re2o = mod1.* sin(2*pi*f0*2^(ScaleTable(16)/12)*t1); 
% mi2o = mod1.* sin(2*pi*f0*2^(ScaleTable(17)/12)*t1); 
% fa2o = mod1.* sin(2*pi*f0*2^(ScaleTable(18)/12)*t1); 
% so2o = mod1.* sin(2*pi*f0*2^(ScaleTable(19)/12)*t1); 
% la2o = mod1.* sin(2*pi*f0*2^(ScaleTable(20)/12)*t1); 
% ti2o = mod1.* sin(2*pi*f0*2^(ScaleTable(21)/12)*t1); 
% blko = zeros(1,a);
% 
% % 2/4 notes  二分音符
% do0t = mod2.* sin(2*pi*f0*2^(ScaleTable(1)/12)*t2); 
% re0t = mod2.* sin(2*pi*f0*2^(ScaleTable(2)/12)*t2);
% mi0t = mod2.* sin(2*pi*f0*2^(ScaleTable(3)/12)*t2);
% fa0t = mod2.* sin(2*pi*f0*2^(ScaleTable(4)/12)*t2);
% so0t = mod2.* sin(2*pi*f0*2^(ScaleTable(5)/12)*t2);
% la0t = mod2.* sin(2*pi*f0*2^(ScaleTable(6)/12)*t2);
% ti0t = mod2.* sin(2*pi*f0*2^(ScaleTable(7)/12)*t2);
% do1t = mod2.* sin(2*pi*f0*2^(ScaleTable(8)/12)*t2);
% re1t = mod2.* sin(2*pi*f0*2^(ScaleTable(9)/12)*t2);
% mi1t = mod2.* sin(2*pi*f0*2^(ScaleTable(10)/12)*t2);
% fa1t = mod2.* sin(2*pi*f0*2^(ScaleTable(11)/12)*t2);
% so1t = mod2.* sin(2*pi*f0*2^(ScaleTable(12)/12)*t2);
% la1t = mod2.* sin(2*pi*f0*2^(ScaleTable(13)/12)*t2);
% ti1t = mod2.* sin(2*pi*f0*2^(ScaleTable(14)/12)*t2);
% do2t = mod2.* sin(2*pi*f0*2^(ScaleTable(15)/12)*t2);
% re2t = mod2.* sin(2*pi*f0*2^(ScaleTable(16)/12)*t2);
% mi2t = mod2.* sin(2*pi*f0*2^(ScaleTable(17)/12)*t2);
% fa2t = mod2.* sin(2*pi*f0*2^(ScaleTable(18)/12)*t2);
% so2f = mod2.* sin(2*pi*f0*2^(ScaleTable(19)/12)*t2);
% la2t = mod2.* sin(2*pi*f0*2^(ScaleTable(20)/12)*t2);
% ti2t = mod2.* sin(2*pi*f0*2^(ScaleTable(21)/12)*t2);
% blkt = zeros(1,b);
% 
% % 1/4 notes  四分音符
% do0f = mod4.*sin(2*pi*f0*2^(ScaleTable(1)/12)*t4); 
% re0f = mod4.*sin(2*pi*f0*2^(ScaleTable(2)/12)*t4); 
% mi0f = mod4.*sin(2*pi*f0*2^(ScaleTable(3)/12)*t4);
% fa0f = mod4.*sin(2*pi*f0*2^(ScaleTable(4)/12)*t4);
% so0f = mod4.*sin(2*pi*f0*2^(ScaleTable(5)/12)*t4);
% la0f = mod4.*sin(2*pi*f0*2^(ScaleTable(6)/12)*t4);
% ti0f = mod4.*sin(2*pi*f0*2^(ScaleTable(7)/12)*t4); 
% do1f = mod4.*sin(2*pi*f0*2^(ScaleTable(8)/12)*t4); 
% re1f = mod4.*sin(2*pi*f0*2^(ScaleTable(9)/12)*t4); 
% mi1f = mod4.*sin(2*pi*f0*2^(ScaleTable(10)/12)*t4); 
% fa1f = mod4.*sin(2*pi*f0*2^(ScaleTable(11)/12)*t4); 
% so1f = mod4.*sin(2*pi*f0*2^(ScaleTable(12)/12)*t4); 
% la1f = mod4.*sin(2*pi*f0*2^(ScaleTable(13)/12)*t4);  
% ti1f = mod4.*sin(2*pi*f0*2^(ScaleTable(14)/12)*t4); 
% do2f = mod4.*sin(2*pi*f0*2^(ScaleTable(15)/12)*t4); 
% re2f = mod4.*sin(2*pi*f0*2^(ScaleTable(16)/12)*t4); 
% mi2f = mod4.*sin(2*pi*f0*2^(ScaleTable(17)/12)*t4); 
% fa2f = mod4.*sin(2*pi*f0*2^(ScaleTable(18)/12)*t4); 
% so2f = mod4.*sin(2*pi*f0*2^(ScaleTable(19)/12)*t4); 
% la2f = mod4.*sin(2*pi*f0*2^(ScaleTable(20)/12)*t4); 
% ti2f = mod4.*sin(2*pi*f0*2^(ScaleTable(21)/12)*t4); 
% blkf = zeros(1,i);
% % 1/8 notes  八分音符
% do0e = mod8.*sin(2*pi*f0*2^(ScaleTable(1)/12)*t8); 
% re0e = mod8.*sin(2*pi*f0*2^(ScaleTable(2)/12)*t8); 
% mi0e = mod8.*sin(2*pi*f0*2^(ScaleTable(3)/12)*t8); 
% fa0e = mod8.*sin(2*pi*f0*2^(ScaleTable(4)/12)*t8); 
% so0e = mod8.*sin(2*pi*f0*2^(ScaleTable(5)/12)*t8); 
% la0e = mod8.*sin(2*pi*f0*2^(ScaleTable(6)/12)*t8); 
% ti0e = mod8.*sin(2*pi*f0*2^(ScaleTable(7)/12)*t8); 
% do1e = mod8.*sin(2*pi*f0*2^(ScaleTable(8)/12)*t8); 
% re1e = mod8.*sin(2*pi*f0*2^(ScaleTable(9)/12)*t8); 
% mi1e = mod8.*sin(2*pi*f0*2^(ScaleTable(10)/12)*t8); 
% fa1e = mod8.*sin(2*pi*f0*2^(ScaleTable(11)/12)*t8); 
% so1e = mod8.*sin(2*pi*f0*2^(ScaleTable(12)/12)*t8); 
% la1e = mod8.*sin(2*pi*f0*2^(ScaleTable(13)/12)*t8);  
% ti1e = mod8.*sin(2*pi*f0*2^(ScaleTable(14)/12)*t8); 
% do2e = mod8.*sin(2*pi*f0*2^(ScaleTable(15)/12)*t8); 
% re2e = mod8.*sin(2*pi*f0*2^(ScaleTable(16)/12)*t8); 
% mi2e = mod8.*sin(2*pi*f0*2^(ScaleTable(17)/12)*t8); 
% fa2e = mod8.*sin(2*pi*f0*2^(ScaleTable(18)/12)*t8); 
% so2e = mod8.*sin(2*pi*f0*2^(ScaleTable(19)/12)*t8); 
% la2e = mod8.*sin(2*pi*f0*2^(ScaleTable(20)/12)*t8); 
% ti2e = mod8.*sin(2*pi*f0*2^(ScaleTable(21)/12)*t8); 
% blke = zeros(1,j); 
% 
% % 1/16 notes   十六分音符
% do0s = mod16.*sin(2*pi*f0*2^(ScaleTable(1)/12)*t16);  
% re0s = mod16.*sin(2*pi*f0*2^(ScaleTable(2)/12)*t16); 
% mi0s = mod16.*sin(2*pi*f0*2^(ScaleTable(3)/12)*t16); 
% fa0s = mod16.*sin(2*pi*f0*2^(ScaleTable(4)/12)*t16);  
% so0s = mod16.*sin(2*pi*f0*2^(ScaleTable(5)/12)*t16);  
% la0s = mod16.*sin(2*pi*f0*2^(ScaleTable(6)/12)*t16);  
% ti0s = mod16.*sin(2*pi*f0*2^(ScaleTable(7)/12)*t16); 
% do1s = mod16.*sin(2*pi*f0*2^(ScaleTable(8)/12)*t16); 
% re1s = mod16.*sin(2*pi*f0*2^(ScaleTable(9)/12)*t16);  
% mi1s = mod16.*sin(2*pi*f0*2^(ScaleTable(10)/12)*t16); 
% fa1s = mod16.*sin(2*pi*f0*2^(ScaleTable(11)/12)*t16); 
% so1s = mod16.*sin(2*pi*f0*2^(ScaleTable(12)/12)*t16); 
% la1s = mod16.*sin(2*pi*f0*2^(ScaleTable(13)/12)*t16); 
% ti1s = mod16.*sin(2*pi*f0*2^(ScaleTable(14)/12)*t16); 
% do2s = mod16.*sin(2*pi*f0*2^(ScaleTable(15)/12)*t16); 
% re2s = mod16.*sin(2*pi*f0*2^(ScaleTable(16)/12)*t16); 
% mi2s = mod16.*sin(2*pi*f0*2^(ScaleTable(17)/12)*t16); 
% fa2s = mod16.*sin(2*pi*f0*2^(ScaleTable(18)/12)*t16); 
% so2s = mod16.*sin(2*pi*f0*2^(ScaleTable(19)/12)*t16); 
% la2s = mod16.*sin(2*pi*f0*2^(ScaleTable(20)/12)*t16); 
% ti2s = mod16.*sin(2*pi*f0*2^(ScaleTable(21)/12)*t16); 
% blks = zeros(1,k); 
% 
% % 管弦乐全体    啊...
% part1=[do2o        la1t  do2t      ti1t  blkf la0e ti1s    so1o ...
%        la1t  do2t  ti1t  so1t      mi1t  blkf re1f         do1t  do1e  blke blkf...
% ];
% 
% % 钱塘澎湃/文理通观
% part2=[so1t so0f la1e  mi1f re0e mi1s do1t    fa1e fa1f mi1e re1f mi0e do1s  so1o...
%        do2t la0f do2e  fa1f so1f la1t         la0e la1s so1e la1e fa1f so1e la1e re1o...
%        so1t so0f la1e  mi1f re0e mi1s do1f do1e do1e    fa0f do1e fa1f do2f la1o...
%        do2f do2e la1f fa1f  so1f do2f mi1t    re0f so0e re1f mi0e mi1s  do1o...
%        ];
%    
% % 同学啊同学少年，青春作伴莫等闲。
% part3=[do2t do2f mi1e       ti1e ti1e la1e so1e la1t         la1e la1e la1e do2e ti1f la0e ti1s  so1o...
%        re0f mi1e fa1f la1f  so0e so1s so1e la1e mi0f blke    re0e re1s mi0e mi1s so1f la1f       ti1o...
%        ];
% 
% % 同学啊同学少年,春华秋实我们奉献。（前）
% part4=[do2t do2f mi1e  ti1e ti1e la1e so1e la1t    la1e la1e la1e do2e ti1e ti1e la0e ti1s  so1o...
%        re0f mi1e fa1e la1e blkf  so0e so1s so1e la1e mi0f blke    do2e do2s so1e la1e so1e fa1e mi1e re1e do1o...
%        ];
% 
% % 同学啊同学少年,春华秋实我们奉献。（后）
% part5=[do2t do2f mi1e  ti1e ti1e la1e so1e la1t    re2e re2e re2e do2e ti1e ti1e la0e ti1s so1o...
%        re2f re1e mi1e fa1e la1e blkf  so0e so1s so1e la1e mi1f blkf    ti1e ti1e ti1e la1e la1f so1f  re2f re2t blkf...
%        do2o do2o do2e blke blkf blkf blkf...
%        ];
%    
% % 全歌
% v1=[part1 part2 part3 part4 part1 part2 part3 part5];
% s=v1;
% s=s/max(s);
% sound(s,fs);









function [note]=pianoNotes(string,Nquarters)
 
%sampling frequency
 
fs = 44100;                           % sample rate 采样率
dt = 1/fs; 
 
T16 = 0.125;                          %十六分之一拍的时间
 
t16 = [0:dt:T16]; 
 
[temp k] = size(t16); 
 
tn = linspace(0,Nquarters*T16,Nquarters*k);
 
% Modification functions  包络函数
%mod=(tn.^4).*exp(-15*(tn.^0.5)); 
%mod=mod*(1/max(mod));         % 调整，使范围控制在0-1
mod=sin(pi*tn/tn(end));
 
if(strcmp(string,'blk')==1)
 
 % a blank note 
 note=zeros(1,fs*Nquarters/4);
else
 % notes table 
switch(string)
 case 'C0'
 n=-21;
 case 'D0'
 n=-19;
 case 'E0'
 n=-17;
 case 'F0'
 n=-15; 
 case 'G0'
 n=-14;
 case 'A0'
 n=-12; 
 case 'B0'
 n=-10;
 case 'C1'
 n=-9;
 case 'D1'
 n=-7;
 case 'E1'
 n=-5;
 case 'F1'
 n=-4;
 case 'F1#'
 n=-3;
 case 'G1'
 n=-2;
 case 'G1#'
 n=-1; 
 case 'A1'
 n=0;
 case 'B1'
 n=2;
 case 'C2'
 n=3;
 case 'D2'
 n=5;
 case 'E2'
 n=7;
 case 'F2'
 n=8;
 case 'G2'
 n=10; 
 case 'A2'
 n=12; 
 case 'B2'
 n=14;
end 
 
 
f0 = 440; % The A string reference frequency
 
%普通正弦波
%note = mod.* (sin(2*pi*f0*2^(n/12)*tn);
 
%普通电子琴
%note = mod.* (sin(2*pi*f0*2^(n/12)*tn)+...
%    0.2*sin(4*pi*f0*2^(n/12)*tn)+...
%    0.3*sin(6*pi*f0*2^(n/12)*tn));

%锯齿波
%note = mod.* (sin(2*pi*f0*2^(n/12)*tn)-...
%    (1/2)*sin(4*pi*f0*2^(n/12)*tn)+...
%    (1/3)*sin(6*pi*f0*2^(n/12)*tn)-...
%    (1/4)*sin(8*pi*f0*2^(n/12)*tn)+...
%    (1/5)*sin(10*pi*f0*2^(n/12)*tn)-...
%    (1/6)*sin(12*pi*f0*2^(n/12)*tn)+...
%    (1/7)*sin(14*pi*f0*2^(n/12)*tn)-...
%    (1/8)*sin(16*pi*f0*2^(n/12)*tn)+...
%    (1/9)*sin(18*pi*f0*2^(n/12)*tn));
 

%矩形波
%note = mod.* (sin(2*pi*f0*2^(n/12)*tn)+...
%    (1/3)*sin(6*pi*f0*2^(n/12)*tn)+...
%    (1/5)*sin(10*pi*f0*2^(n/12)*tn)+...
%    (1/7)*sin(14*pi*f0*2^(n/12)*tn)+...
%    (1/9)*sin(18*pi*f0*2^(n/12)*tn)+...
%    (1/11)*sin(22*pi*f0*2^(n/12)*tn)+...
%    (1/13)*sin(26*pi*f0*2^(n/12)*tn)+...
%    (1/15)*sin(30*pi*f0*2^(n/12)*tn)+...
%    (1/17)*sin(34*pi*f0*2^(n/12)*tn));
 
%三角波
note = mod.* (cos(2*pi*f0*2^(n/12)*tn)+...
    (1/3^2)*cos(6*pi*f0*2^(n/12)*tn)+...
    (1/5^2)*cos(10*pi*f0*2^(n/12)*tn)+...
    (1/7^2)*cos(14*pi*f0*2^(n/12)*tn)+...
    (1/9^2)*cos(18*pi*f0*2^(n/12)*tn)+...
    (1/11^2)*cos(22*pi*f0*2^(n/12)*tn)+...
    (1/13^2)*cos(26*pi*f0*2^(n/12)*tn)+...
    (1/15^2)*cos(30*pi*f0*2^(n/12)*tn)+...
(1/17^2)*cos(34*pi*f0*2^(n/12)*tn));
%您可以去掉注释，选择您喜欢的波形
note=note*(1/max(note));
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%将分隔行以上单独保存为“pianoNotes.m”进行函数的调用
%以下为程序代码
% Castle in the Sky-- The girl who fell from the sky
 
% sampling frequency
fs=44100;
% melodies
%pianoNotes('音名',音长)
% 前奏。。。在我的怀。。。
part1 = [pianoNotes('A0',2) pianoNotes('B0',2) pianoNotes('C1',4) pianoNotes('G1',4) pianoNotes('F1',16)...
 pianoNotes('blk',4) pianoNotes('G0',2) pianoNotes('A0',2) pianoNotes('B0',4) pianoNotes('F1',4) pianoNotes('E1',16)...
 pianoNotes('blk',4) pianoNotes('E1',2) pianoNotes('E1',2) pianoNotes('A1',4) pianoNotes('G1',4) pianoNotes('F1',8) pianoNotes('D1',8)...
 pianoNotes('D1',4) pianoNotes('B0',4) pianoNotes('E1',4) pianoNotes('D1',4) pianoNotes('A1',16)...
 pianoNotes('blk',4) pianoNotes('A0',2) pianoNotes('B0',2) pianoNotes('C1',4) pianoNotes('G1',4)
];
 %。。。里，在你的眼里。。。。。。多少年以。。。
part2 = [ 
 pianoNotes('D1',8) pianoNotes('blk',12) pianoNotes('G0',2) pianoNotes('A0',2) pianoNotes('B0',4) pianoNotes('F1',4) pianoNotes('E1',8) pianoNotes('blk',12) pianoNotes('E1',2) pianoNotes('E1',2) pianoNotes('A1',4) pianoNotes('G1',4) pianoNotes('F1',4) pianoNotes('D1',8) pianoNotes('blk',2) pianoNotes('B0',2)...
 pianoNotes('B0',4) pianoNotes('C1',2) pianoNotes('D1',2) pianoNotes('D1',4) pianoNotes('F1',4) pianoNotes('E1',12) pianoNotes('blk',8) pianoNotes('A0',2) pianoNotes('B0',2) pianoNotes('C1',4) pianoNotes('G1',4)...
 pianoNotes('D1',8) pianoNotes('blk',12) pianoNotes('G0',2) pianoNotes('A0',2) pianoNotes('B0',4) pianoNotes('F1',4) pianoNotes('E1',8) pianoNotes('blk',12) pianoNotes('E1',2) pianoNotes('E1',2) pianoNotes('A1',4) pianoNotes('G1',4) pianoNotes('F1',4) pianoNotes('D1',8) pianoNotes('blk',2) pianoNotes('B0',2)...
 pianoNotes('B0',4) pianoNotes('E1',2) pianoNotes('D1',2) pianoNotes('D1',4) pianoNotes('C1',2) pianoNotes('B0',2) pianoNotes('A0',4) pianoNotes('A0',2) pianoNotes('A0',2) pianoNotes('A0',4) pianoNotes('A1',4) 
];
 %后，如云般游走。。。湖畔。。。（公共部分）
part3 = [ 
 pianoNotes('A1',12) pianoNotes('blk',8) pianoNotes('A0',2) pianoNotes('A1',2) pianoNotes('G1',4) pianoNotes('G1',1) pianoNotes('A1',1) pianoNotes('G1',2) pianoNotes('E1',12) pianoNotes('blk',4)...
 pianoNotes('blk',4) pianoNotes('E1',2) pianoNotes('E1',2) pianoNotes('A1',4) pianoNotes('G1',4) pianoNotes('F1',4) pianoNotes('D1',8) pianoNotes('blk',2) pianoNotes('C1',2) pianoNotes('B0',4) pianoNotes('C1',2) pianoNotes('D1',2) pianoNotes('D1',4) pianoNotes('F1',4) pianoNotes('E1',12) pianoNotes('blk',4)... 
 pianoNotes('blk',4) pianoNotes('A0',2) pianoNotes('A0',2) pianoNotes('A0',4) pianoNotes('A1',4) pianoNotes('A1',12) pianoNotes('blk',4) pianoNotes('blk',4) pianoNotes('B1',2) pianoNotes('C2',2) pianoNotes('D2',4) pianoNotes('C2',4) pianoNotes('E1',12) pianoNotes('blk',4)...
 pianoNotes('blk',4) pianoNotes('E1',2) pianoNotes('E1',2) pianoNotes('A1',4) pianoNotes('G1',4) pianoNotes('F1',4) pianoNotes('D1',8) pianoNotes('blk',4) pianoNotes('G1',4) pianoNotes('A1',2) pianoNotes('B1',2) pianoNotes('B1',4) pianoNotes('B1',4) pianoNotes('E2',16) pianoNotes('B1',16) pianoNotes('blk',16)...
 pianoNotes('blk',4) pianoNotes('A0',2) pianoNotes('B0',2) pianoNotes('C1',4) pianoNotes('G1',4) pianoNotes('F1',8) pianoNotes('blk',12) pianoNotes('G0',2) pianoNotes('A0',2) pianoNotes('B0',4) pianoNotes('F1',4) pianoNotes('E1',8) pianoNotes('blk',8)...
 pianoNotes('blk',4) pianoNotes('E1',2) pianoNotes('E1',2) pianoNotes('A1',4) pianoNotes('G1',2) pianoNotes('F1',2) pianoNotes('F1',2) pianoNotes('D1',2) pianoNotes('D1',8) pianoNotes('blk',2) pianoNotes('B0',2) pianoNotes('B0',4) pianoNotes('E1',2) pianoNotes('D1',2) pianoNotes('D1',4) pianoNotes('B0',2) pianoNotes('C0',2) pianoNotes('A0',12) pianoNotes('blk',4)
];
 %多少年以。。。
part4 = [ 
 pianoNotes('blk',4) pianoNotes('A0',2) pianoNotes('A0',2) pianoNotes('A0',4) pianoNotes('A1',4) 
];
 %结尾
part5 = [
 pianoNotes('blk',4) pianoNotes('E1',2) pianoNotes('E1',2) pianoNotes('A1',4) pianoNotes('G1',4) pianoNotes('F1',4) pianoNotes('D1',8) pianoNotes('blk',2) pianoNotes('B0',2) pianoNotes('B0',4) pianoNotes('E1',2) pianoNotes('D1',2) pianoNotes('D1',4) pianoNotes('A0',2) pianoNotes('C0',2) pianoNotes('A0',12) pianoNotes('blk',4)
];
 %播放
 s = [ part1 part2 part3 part4 part3 part5];
 sound(s,fs);


