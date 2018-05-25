left=[1,2,3];
right=[2,6,8];
road_width1=right-left;
fid=fopen('E:\github\zsp_updata\data\road_width.txt','w');
fprintf(fid,'%d,',road_width1);
fclose(fid);