#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

const inline double deg(double rad) {
    return(180.0*M_1_PI*rad);
}

const inline double rad(double deg) {
    return(M_PI/180.0*deg);
}

void cvtJ2000toGalactic(double ra,double dec,double *pl,double *pb) {
    /*
     ** Constants defining the galactiv plane taken from the appendix of a 
     ** paper investigating the motion of Sgr A*.
     ** http://lanl.arxiv.org/abs/astro-ph/0408107
     */
    const double rgc=rad(192.859508335),dgc=rad(27.1283361111),lcp=rad(122.932);
    double sin_t,cos_t,t;

    *pb=asin(sin(dgc)*sin(dec)+cos(dgc)*cos(dec)*(cos(ra-rgc)));
    sin_t=(cos(dec)*sin(ra-rgc)/cos(*pb));
    cos_t=((cos(dgc)*sin(dec)-sin(dgc)*cos(dec)*cos(ra-rgc))/cos(*pb));
    t = atan2(sin_t,cos_t);
    *pl=lcp-t;
    /*
     ** Ensure l lies in the range 0-2pi  
     */
    if (*pl < 0.0) *pl += 2.0*M_PI;
    if (*pl >= 2.0*M_PI) *pl -= 2.0*M_PI;
}

int main(void) {
    FILE *hip2 = fopen("hip2_sorted.dat","r");
    FILE *hip = fopen("hip_main_sorted.dat","r");
    const int nHip2 = 278;
    char achHip2[nHip2];
    const int nFieldsHip2 = 27;
    const int sepHip2[] = {6,10,12,14,28,42,50,59,68,75,82,
        89,96,103,107,113,116,123,128,136,
        143,149,151,158,164,171,276};
    const int nHip = 1024;
    char achHip[nHip];
    const int nFieldsHip = 78;
    char *p;
    char *tok,*tok1;
    int i;
    double ra,dec,plx,e_plx,hipMag,BminusV,VminusI;
    double l,b,r,x,y,z;
    int idHip,nComp,ihip,iVar,iSolution;

    assert(hip2 != NULL);
    assert(hip != NULL);

    p = fgets(achHip,nHip,hip);
    tok1 = strsep(&p,"|");
    assert(tok1 != NULL);
    tok1 = strsep(&p,"|");
    assert(tok1 != NULL);
    while (1) {
        tok = fgets(achHip2,nHip2,hip2);
        if (tok == NULL) break;
        for (i=0;i<nFieldsHip2;++i) {
            achHip2[sepHip2[i]] = 0;
            switch(i) {
                case 0:
                    idHip = atoi(tok);
                    rewind(hip); /* TMU: since my data is not sorted */
                    while (1) {
                        if (atoi(tok1) == idHip) break;
                        p = fgets(achHip,nHip,hip);
                        tok1 = strsep(&p,"|");
                        assert(tok1 != NULL);
                        tok1 = strsep(&p,"|");
                        assert(tok1 != NULL);
                    }
                    break;
                case 1:
                    iSolution = atoi(tok); break; 
                case 3:
                    nComp = atoi(tok); break;
                case 4:
                    ra = atof(tok); break;
                case 5:
                    dec = atof(tok); break;
                case 6:
                    plx = atof(tok); break;
                case 11:
                    e_plx = atof(tok); break;
                case 19:
                    hipMag = atof(tok); break;
                case 22:
                    iVar = atoi(tok); break;
                case 23:
                    BminusV = atof(tok); break;
                case 25:
                    VminusI = atof(tok); break;
            }
            tok = &achHip2[sepHip2[i]+1];
        }
        assert(*tok == 0);
        /*
         ** Skip all double stars for now...
         */
        if (nComp > 1) continue;

        cvtJ2000toGalactic(ra,dec,&l,&b);
        if (plx < 0.5*e_plx) r = 1.5/e_plx;  /* distance is very uncertain */
        else r = 1/plx;
        x = r*cos(b)*cos(l);
        y = r*cos(b)*sin(l);
        z = r*sin(b);
        printf("%6d,%13.10f,%13.10f,%13.10f,%7.4f,%1d,%6.3f,%6.3f",idHip,x,y,z,hipMag,iVar,BminusV,VminusI);
        /*
         ** Get remaining fields from the original Hipparchos catalog.
         */
        for (ihip=2;ihip<nFieldsHip;++ihip) {
            tok1 = strsep(&p,"|");
            assert(tok1 != NULL);
            switch (ihip) {
                case 49:
                    printf(",%s",tok1); break;
                case 50:
                    printf(",%s",tok1); break;
                case 51:
                    printf(",%s",tok1); break;
                case 52:
                    printf(",%s",tok1); break;
                    /*
                     ** For double star systems.
                     case 64:
                     printf(",%s",tok1); break;
                     case 66:
                     printf(",%s",tok1); break;
                     */
                case 71:
                    printf(",%s",tok1); break;
                case 76:
                    printf(",%s",tok1); break;
            }
        }
        printf("\n");
    }
    fclose(hip2);
    fclose(hip);
    return 0;
}
