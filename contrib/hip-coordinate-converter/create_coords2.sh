awk '$4==1{cvtr=3.141592654/180.0;a=$5*cvtr;d=$6*cvtr;c1=62.6*cvtr;c2=282.25*cvtr;x=cos(d)*cos(a-c2);y=sin(d)*sin(c1)+cos(d)*sin(a-c2)*cos(c1);z=sin(d)*cos(c1)-cos(d)*sin(a-c2)*sin(c1);l=atan2(y,x)/cvtr+33.0;b=atan2(z,sqrt(x*x + y*y))/cvtr;print $1," ",$5," ",$6," ",$7," ",$12," ",l," ",b}' hip2.dat

