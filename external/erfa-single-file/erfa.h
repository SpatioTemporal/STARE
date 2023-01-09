#ifndef ERFAHDEF
#define ERFAHDEF

#include <math.h>

/*
**  - - - - - - -
**   e r f a . h
**  - - - - - - -
**
**  Prototype function declarations and macros for erfa library.
**
*/

/* Pi */
#define ERFA_DPI (3.141592653589793238462643)

/* 2Pi */
#define ERFA_D2PI (6.283185307179586476925287)

/* Radians to degrees */
#define ERFA_DR2D (57.29577951308232087679815)

/* Degrees to radians */
#define ERFA_DD2R (1.745329251994329576923691e-2)

/* Radians to arcseconds */
#define ERFA_DR2AS (206264.8062470963551564734)

/* Arcseconds to radians */
#define ERFA_DAS2R (4.848136811095359935899141e-6)

/* Seconds of time to radians */
#define ERFA_DS2R (7.272205216643039903848712e-5)

/* Arcseconds in a full circle */
#define ERFA_TURNAS (1296000.0)

/* Milliarcseconds to radians */
#define ERFA_DMAS2R (ERFA_DAS2R / 1e3)

/* Length of tropical year B1900 (days) */
#define ERFA_DTY (365.242198781)

/* Seconds per day. */
#define ERFA_DAYSEC (86400.0)

/* Days per Julian year */
#define ERFA_DJY (365.25)

/* Days per Julian century */
#define ERFA_DJC (36525.0)

/* Days per Julian millennium */
#define ERFA_DJM (365250.0)

/* Reference epoch (J2000.0), Julian Date */
#define ERFA_DJ00 (2451545.0)

/* Julian Date of Modified Julian Date zero */
#define ERFA_DJM0 (2400000.5)

/* Reference epoch (J2000.0), Modified Julian Date */
#define ERFA_DJM00 (51544.5)

/* 1977 Jan 1.0 as MJD */
#define ERFA_DJM77 (43144.0)

/* TT minus TAI (s) */
#define ERFA_TTMTAI (32.184)

/* Astronomical unit (m, IAU 2012) */
#define ERFA_DAU (149597870.7e3)

/* Speed of light (m/s) */
#define ERFA_CMPS 299792458.0

/* Light time for 1 au (s) */
#define ERFA_AULT (ERFA_DAU/ERFA_CMPS)

/* Speed of light (au per day) */
#define ERFA_DC (ERFA_DAYSEC/ERFA_AULT)

/* L_G = 1 - d(TT)/d(TCG) */
#define ERFA_ELG (6.969290134e-10)

/* L_B = 1 - d(TDB)/d(TCB), and TDB (s) at TAI 1977/1/1.0 */
#define ERFA_ELB (1.550519768e-8)
#define ERFA_TDB0 (-6.55e-5)

/* Schwarzschild radius of the Sun (au) */
/* = 2 * 1.32712440041e20 / (2.99792458e8)^2 / 1.49597870700e11 */
#define ERFA_SRS 1.97412574336e-8

/* ERFA_DINT(A) - truncate to nearest whole number towards zero (double) */
#define ERFA_DINT(A) ((A)<0.0?ceil(A):floor(A))

/* ERFA_DNINT(A) - round to nearest whole number (double) */
#define ERFA_DNINT(A) (fabs(A)<0.5?0.0\
                                :((A)<0.0?ceil((A)-0.5):floor((A)+0.5)))

/* ERFA_DSIGN(A,B) - magnitude of A with sign of B (double) */
#define ERFA_DSIGN(A,B) ((B)<0.0?-fabs(A):fabs(A))

/* max(A,B) - larger (most +ve) of two numbers (generic) */
#define ERFA_GMAX(A,B) (((A)>(B))?(A):(B))

/* min(A,B) - smaller (least +ve) of two numbers (generic) */
#define ERFA_GMIN(A,B) (((A)<(B))?(A):(B))

/* Reference ellipsoids */
#define ERFA_WGS84 1
#define ERFA_GRS80 2
#define ERFA_WGS72 3


#ifdef __cplusplus
extern "C" {
#endif

/* Star-independent astrometry parameters */
typedef struct {
   double pmt;        /* PM time interval (SSB, Julian years) */
   double eb[3];      /* SSB to observer (vector, au) */
   double eh[3];      /* Sun to observer (unit vector) */
   double em;         /* distance from Sun to observer (au) */
   double v[3];       /* barycentric observer velocity (vector, c) */
   double bm1;        /* sqrt(1-|v|^2): reciprocal of Lorenz factor */
   double bpn[3][3];  /* bias-precession-nutation matrix */
   double along;      /* longitude + s' + dERA(DUT) (radians) */
   double phi;        /* geodetic latitude (radians) */
   double xpl;        /* polar motion xp wrt local meridian (radians) */
   double ypl;        /* polar motion yp wrt local meridian (radians) */
   double sphi;       /* sine of geodetic latitude */
   double cphi;       /* cosine of geodetic latitude */
   double diurab;     /* magnitude of diurnal aberration vector */
   double eral;       /* "local" Earth rotation angle (radians) */
   double refa;       /* refraction constant A (radians) */
   double refb;       /* refraction constant B (radians) */
} eraASTROM;
/* (Vectors eb, eh, em and v are all with respect to BCRS axes.) */

/* Body parameters for light deflection */
typedef struct {
   double bm;         /* mass of the body (solar masses) */
   double dl;         /* deflection limiter (radians^2/2) */
   double pv[2][3];   /* barycentric PV of the body (au, au/day) */
} eraLDBODY;

/* Astronomy/Calendars */
int eraCal2jd(int iy, int im, int id, double *djm0, double *djm);
double eraEpb(double dj1, double dj2);
void eraEpb2jd(double epb, double *djm0, double *djm);
double eraEpj(double dj1, double dj2);
void eraEpj2jd(double epj, double *djm0, double *djm);
int eraJd2cal(double dj1, double dj2,
                     int *iy, int *im, int *id, double *fd);
int eraJdcalf(int ndp, double dj1, double dj2, int iymdf[4]);

/* Astronomy/Astrometry */
void eraAb(double pnat[3], double v[3], double s, double bm1,
           double ppr[3]);
void eraApcg(double date1, double date2,
             double ebpv[2][3], double ehp[3],
             eraASTROM *astrom);
void eraApcg13(double date1, double date2, eraASTROM *astrom);
void eraApci(double date1, double date2,
             double ebpv[2][3], double ehp[3],
             double x, double y, double s,
             eraASTROM *astrom);
void eraApci13(double date1, double date2,
               eraASTROM *astrom, double *eo);
void eraApco(double date1, double date2,
             double ebpv[2][3], double ehp[3],
             double x, double y, double s, double theta,
             double elong, double phi, double hm,
             double xp, double yp, double sp,
             double refa, double refb,
             eraASTROM *astrom);
int eraApco13(double utc1, double utc2, double dut1,
              double elong, double phi, double hm, double xp, double yp,
              double phpa, double tc, double rh, double wl,
              eraASTROM *astrom, double *eo);
void eraApcs(double date1, double date2, double pv[2][3],
             double ebpv[2][3], double ehp[3],
             eraASTROM *astrom);
void eraApcs13(double date1, double date2, double pv[2][3],
               eraASTROM *astrom);
void eraAper(double theta, eraASTROM *astrom);
void eraAper13(double ut11, double ut12, eraASTROM *astrom);
void eraApio(double sp, double theta,
             double elong, double phi, double hm, double xp, double yp,
             double refa, double refb,
             eraASTROM *astrom);
int eraApio13(double utc1, double utc2, double dut1,
              double elong, double phi, double hm, double xp, double yp,
              double phpa, double tc, double rh, double wl,
              eraASTROM *astrom);
void eraAtcc13(double rc, double dc,
               double pr, double pd, double px, double rv,
               double date1, double date2,
               double *ra, double *da);
void eraAtccq(double rc, double dc,
              double pr, double pd, double px, double rv,
              eraASTROM *astrom, double *ra, double *da);
void eraAtci13(double rc, double dc,
               double pr, double pd, double px, double rv,
               double date1, double date2,
               double *ri, double *di, double *eo);
void eraAtciq(double rc, double dc, double pr, double pd,
              double px, double rv, eraASTROM *astrom,
              double *ri, double *di);
void eraAtciqn(double rc, double dc, double pr, double pd,
               double px, double rv, eraASTROM *astrom,
               int n, eraLDBODY b[], double *ri, double *di);
void eraAtciqz(double rc, double dc, eraASTROM *astrom,
               double *ri, double *di);
int eraAtco13(double rc, double dc,
              double pr, double pd, double px, double rv,
              double utc1, double utc2, double dut1,
              double elong, double phi, double hm, double xp, double yp,
              double phpa, double tc, double rh, double wl,
              double *aob, double *zob, double *hob,
              double *dob, double *rob, double *eo);
void eraAtic13(double ri, double di,
               double date1, double date2,
               double *rc, double *dc, double *eo);
void eraAticq(double ri, double di, eraASTROM *astrom,
              double *rc, double *dc);
void eraAticqn(double ri, double di, eraASTROM *astrom,
               int n, eraLDBODY b[], double *rc, double *dc);
int eraAtio13(double ri, double di,
              double utc1, double utc2, double dut1,
              double elong, double phi, double hm, double xp, double yp,
              double phpa, double tc, double rh, double wl,
              double *aob, double *zob, double *hob,
              double *dob, double *rob);
void eraAtioq(double ri, double di, eraASTROM *astrom,
              double *aob, double *zob,
              double *hob, double *dob, double *rob);
int eraAtoc13(const char *type, double ob1, double ob2,
              double utc1, double utc2, double dut1,
              double elong, double phi, double hm, double xp, double yp,
              double phpa, double tc, double rh, double wl,
              double *rc, double *dc);
int eraAtoi13(const char *type, double ob1, double ob2,
              double utc1, double utc2, double dut1,
              double elong, double phi, double hm, double xp, double yp,
              double phpa, double tc, double rh, double wl,
              double *ri, double *di);
void eraAtoiq(const char *type,
              double ob1, double ob2, eraASTROM *astrom,
              double *ri, double *di);
void eraLd(double bm, double p[3], double q[3], double e[3],
           double em, double dlim, double p1[3]);
void eraLdn(int n, eraLDBODY b[], double ob[3], double sc[3],
            double sn[3]);
void eraLdsun(double p[3], double e[3], double em, double p1[3]);
void eraPmpx(double rc, double dc, double pr, double pd,
             double px, double rv, double pmt, double pob[3],
             double pco[3]);
int eraPmsafe(double ra1, double dec1, double pmr1, double pmd1,
              double px1, double rv1,
              double ep1a, double ep1b, double ep2a, double ep2b,
              double *ra2, double *dec2, double *pmr2, double *pmd2,
              double *px2, double *rv2);
void eraPvtob(double elong, double phi, double height, double xp,
              double yp, double sp, double theta, double pv[2][3]);
void eraRefco(double phpa, double tc, double rh, double wl,
              double *refa, double *refb);

/* Astronomy/Ephemerides */
int eraEpv00(double date1, double date2,
             double pvh[2][3], double pvb[2][3]);
void eraMoon98(double date1, double date2, double pv[2][3]);
int eraPlan94(double date1, double date2, int np, double pv[2][3]);

/* Astronomy/FundamentalArgs */
double eraFad03(double t);
double eraFae03(double t);
double eraFaf03(double t);
double eraFaju03(double t);
double eraFal03(double t);
double eraFalp03(double t);
double eraFama03(double t);
double eraFame03(double t);
double eraFane03(double t);
double eraFaom03(double t);
double eraFapa03(double t);
double eraFasa03(double t);
double eraFaur03(double t);
double eraFave03(double t);

/* Astronomy/PrecNutPolar */
void eraBi00(double *dpsibi, double *depsbi, double *dra);
void eraBp00(double date1, double date2,
             double rb[3][3], double rp[3][3], double rbp[3][3]);
void eraBp06(double date1, double date2,
             double rb[3][3], double rp[3][3], double rbp[3][3]);
void eraBpn2xy(double rbpn[3][3], double *x, double *y);
void eraC2i00a(double date1, double date2, double rc2i[3][3]);
void eraC2i00b(double date1, double date2, double rc2i[3][3]);
void eraC2i06a(double date1, double date2, double rc2i[3][3]);
void eraC2ibpn(double date1, double date2, double rbpn[3][3],
               double rc2i[3][3]);
void eraC2ixy(double date1, double date2, double x, double y,
              double rc2i[3][3]);
void eraC2ixys(double x, double y, double s, double rc2i[3][3]);
void eraC2t00a(double tta, double ttb, double uta, double utb,
               double xp, double yp, double rc2t[3][3]);
void eraC2t00b(double tta, double ttb, double uta, double utb,
               double xp, double yp, double rc2t[3][3]);
void eraC2t06a(double tta, double ttb, double uta, double utb,
               double xp, double yp, double rc2t[3][3]);
void eraC2tcio(double rc2i[3][3], double era, double rpom[3][3],
               double rc2t[3][3]);
void eraC2teqx(double rbpn[3][3], double gst, double rpom[3][3],
               double rc2t[3][3]);
void eraC2tpe(double tta, double ttb, double uta, double utb,
              double dpsi, double deps, double xp, double yp,
              double rc2t[3][3]);
void eraC2txy(double tta, double ttb, double uta, double utb,
              double x, double y, double xp, double yp,
              double rc2t[3][3]);
double eraEo06a(double date1, double date2);
double eraEors(double rnpb[3][3], double s);
void eraFw2m(double gamb, double phib, double psi, double eps,
             double r[3][3]);
void eraFw2xy(double gamb, double phib, double psi, double eps,
              double *x, double *y);
void eraLtp(double epj, double rp[3][3]);
void eraLtpb(double epj, double rpb[3][3]);
void eraLtpecl(double epj, double vec[3]);
void eraLtpequ(double epj, double veq[3]);
void eraNum00a(double date1, double date2, double rmatn[3][3]);
void eraNum00b(double date1, double date2, double rmatn[3][3]);
void eraNum06a(double date1, double date2, double rmatn[3][3]);
void eraNumat(double epsa, double dpsi, double deps, double rmatn[3][3]);
void eraNut00a(double date1, double date2, double *dpsi, double *deps);
void eraNut00b(double date1, double date2, double *dpsi, double *deps);
void eraNut06a(double date1, double date2, double *dpsi, double *deps);
void eraNut80(double date1, double date2, double *dpsi, double *deps);
void eraNutm80(double date1, double date2, double rmatn[3][3]);
double eraObl06(double date1, double date2);
double eraObl80(double date1, double date2);
void eraP06e(double date1, double date2,
             double *eps0, double *psia, double *oma, double *bpa,
             double *bqa, double *pia, double *bpia,
             double *epsa, double *chia, double *za, double *zetaa,
             double *thetaa, double *pa,
             double *gam, double *phi, double *psi);
void eraPb06(double date1, double date2,
             double *bzeta, double *bz, double *btheta);
void eraPfw06(double date1, double date2,
              double *gamb, double *phib, double *psib, double *epsa);
void eraPmat00(double date1, double date2, double rbp[3][3]);
void eraPmat06(double date1, double date2, double rbp[3][3]);
void eraPmat76(double date1, double date2, double rmatp[3][3]);
void eraPn00(double date1, double date2, double dpsi, double deps,
             double *epsa,
             double rb[3][3], double rp[3][3], double rbp[3][3],
             double rn[3][3], double rbpn[3][3]);
void eraPn00a(double date1, double date2,
              double *dpsi, double *deps, double *epsa,
              double rb[3][3], double rp[3][3], double rbp[3][3],
              double rn[3][3], double rbpn[3][3]);
void eraPn00b(double date1, double date2,
              double *dpsi, double *deps, double *epsa,
              double rb[3][3], double rp[3][3], double rbp[3][3],
              double rn[3][3], double rbpn[3][3]);
void eraPn06(double date1, double date2, double dpsi, double deps,
             double *epsa,
             double rb[3][3], double rp[3][3], double rbp[3][3],
             double rn[3][3], double rbpn[3][3]);
void eraPn06a(double date1, double date2,
              double *dpsi, double *deps, double *epsa,
              double rb[3][3], double rp[3][3], double rbp[3][3],
              double rn[3][3], double rbpn[3][3]);
void eraPnm00a(double date1, double date2, double rbpn[3][3]);
void eraPnm00b(double date1, double date2, double rbpn[3][3]);
void eraPnm06a(double date1, double date2, double rnpb[3][3]);
void eraPnm80(double date1, double date2, double rmatpn[3][3]);
void eraPom00(double xp, double yp, double sp, double rpom[3][3]);
void eraPr00(double date1, double date2,
             double *dpsipr, double *depspr);
void eraPrec76(double date01, double date02,
               double date11, double date12,
               double *zeta, double *z, double *theta);
double eraS00(double date1, double date2, double x, double y);
double eraS00a(double date1, double date2);
double eraS00b(double date1, double date2);
double eraS06(double date1, double date2, double x, double y);
double eraS06a(double date1, double date2);
double eraSp00(double date1, double date2);
void eraXy06(double date1, double date2, double *x, double *y);
void eraXys00a(double date1, double date2,
               double *x, double *y, double *s);
void eraXys00b(double date1, double date2,
               double *x, double *y, double *s);
void eraXys06a(double date1, double date2,
               double *x, double *y, double *s);

/* Astronomy/RotationAndTime */
double eraEe00(double date1, double date2, double epsa, double dpsi);
double eraEe00a(double date1, double date2);
double eraEe00b(double date1, double date2);
double eraEe06a(double date1, double date2);
double eraEect00(double date1, double date2);
double eraEqeq94(double date1, double date2);
double eraEra00(double dj1, double dj2);
double eraGmst00(double uta, double utb, double tta, double ttb);
double eraGmst06(double uta, double utb, double tta, double ttb);
double eraGmst82(double dj1, double dj2);
double eraGst00a(double uta, double utb, double tta, double ttb);
double eraGst00b(double uta, double utb);
double eraGst06(double uta, double utb, double tta, double ttb,
                double rnpb[3][3]);
double eraGst06a(double uta, double utb, double tta, double ttb);
double eraGst94(double uta, double utb);

/* Astronomy/SpaceMotion */
int eraPvstar(double pv[2][3], double *ra, double *dec,
              double *pmr, double *pmd, double *px, double *rv);
int eraStarpv(double ra, double dec,
              double pmr, double pmd, double px, double rv,
              double pv[2][3]);

/* Astronomy/StarCatalogs */

void eraFk425(double r1950, double d1950,
              double dr1950, double dd1950,
              double p1950, double v1950,
              double *r2000, double *d2000,
              double *dr2000, double *dd2000,
              double *p2000, double *v2000);
void eraFk45z(double r1950, double d1950, double bepoch,
              double *r2000, double *d2000);
void eraFk524(double r2000, double d2000,
              double dr2000, double dd2000,
              double p2000, double v2000,
              double *r1950, double *d1950,
              double *dr1950, double *dd1950,
              double *p1950, double *v1950);
void eraFk52h(double r5, double d5,
              double dr5, double dd5, double px5, double rv5,
              double *rh, double *dh,
              double *drh, double *ddh, double *pxh, double *rvh);
void eraFk54z(double r2000, double d2000, double bepoch,
              double *r1950, double *d1950,
              double *dr1950, double *dd1950);
void eraFk5hip(double r5h[3][3], double s5h[3]);
void eraFk5hz(double r5, double d5, double date1, double date2,
              double *rh, double *dh);
void eraH2fk5(double rh, double dh,
              double drh, double ddh, double pxh, double rvh,
              double *r5, double *d5,
              double *dr5, double *dd5, double *px5, double *rv5);
void eraHfk5z(double rh, double dh, double date1, double date2,
              double *r5, double *d5, double *dr5, double *dd5);
int eraStarpm(double ra1, double dec1,
              double pmr1, double pmd1, double px1, double rv1,
              double ep1a, double ep1b, double ep2a, double ep2b,
              double *ra2, double *dec2,
              double *pmr2, double *pmd2, double *px2, double *rv2);

/* Astronomy/EclipticCoordinates */
void eraEceq06(double date1, double date2, double dl, double db,
               double *dr, double *dd);
void eraEcm06(double date1, double date2, double rm[3][3]);
void eraEqec06(double date1, double date2, double dr, double dd,
               double *dl, double *db);
void eraLteceq(double epj, double dl, double db, double *dr, double *dd);
void eraLtecm(double epj, double rm[3][3]);
void eraLteqec(double epj, double dr, double dd, double *dl, double *db);

/* Astronomy/GalacticCoordinates */
void eraG2icrs(double dl, double db, double *dr, double *dd);
void eraIcrs2g(double dr, double dd, double *dl, double *db);

/* Astronomy/GeodeticGeocentric */
int eraEform(int n, double *a, double *f);
int eraGc2gd(int n, double xyz[3],
             double *elong, double *phi, double *height);
int eraGc2gde(double a, double f, double xyz[3],
              double *elong, double *phi, double *height);
int eraGd2gc(int n, double elong, double phi, double height,
             double xyz[3]);
int eraGd2gce(double a, double f,
              double elong, double phi, double height, double xyz[3]);

/* Astronomy/Timescales */
int eraD2dtf(const char *scale, int ndp, double d1, double d2,
             int *iy, int *im, int *id, int ihmsf[4]);
int eraDat(int iy, int im, int id, double fd, double *deltat);
double eraDtdb(double date1, double date2,
               double ut, double elong, double u, double v);
int eraDtf2d(const char *scale, int iy, int im, int id,
             int ihr, int imn, double sec, double *d1, double *d2);
int eraTaitt(double tai1, double tai2, double *tt1, double *tt2);
int eraTaiut1(double tai1, double tai2, double dta,
              double *ut11, double *ut12);
int eraTaiutc(double tai1, double tai2, double *utc1, double *utc2);
int eraTcbtdb(double tcb1, double tcb2, double *tdb1, double *tdb2);
int eraTcgtt(double tcg1, double tcg2, double *tt1, double *tt2);
int eraTdbtcb(double tdb1, double tdb2, double *tcb1, double *tcb2);
int eraTdbtt(double tdb1, double tdb2, double dtr,
             double *tt1, double *tt2);
int eraTttai(double tt1, double tt2, double *tai1, double *tai2);
int eraTttcg(double tt1, double tt2, double *tcg1, double *tcg2);
int eraTttdb(double tt1, double tt2, double dtr,
             double *tdb1, double *tdb2);
int eraTtut1(double tt1, double tt2, double dt,
             double *ut11, double *ut12);
int eraUt1tai(double ut11, double ut12, double dta,
              double *tai1, double *tai2);
int eraUt1tt(double ut11, double ut12, double dt,
             double *tt1, double *tt2);
int eraUt1utc(double ut11, double ut12, double dut1,
              double *utc1, double *utc2);
int eraUtctai(double utc1, double utc2, double *tai1, double *tai2);
int eraUtcut1(double utc1, double utc2, double dut1,
              double *ut11, double *ut12);

/* Astronomy/HorizonEquatorial */
void eraAe2hd(double az, double el, double phi,
              double *ha, double *dec);
void eraHd2ae(double ha, double dec, double phi,
              double *az, double *el);
double eraHd2pa(double ha, double dec, double phi);

/* Astronomy/Gnomonic */
int eraTpors(double xi, double eta, double a, double b,
             double *a01, double *b01, double *a02, double *b02);
int eraTporv(double xi, double eta, double v[3],
             double v01[3], double v02[3]);
void eraTpsts(double xi, double eta, double a0, double b0,
              double *a, double *b);
void eraTpstv(double xi, double eta, double v0[3], double v[3]);
int eraTpxes(double a, double b, double a0, double b0,
             double *xi, double *eta);
int eraTpxev(double v[3], double v0[3], double *xi, double *eta);

/* VectorMatrix/AngleOps */
void eraA2af(int ndp, double angle, char *sign, int idmsf[4]);
void eraA2tf(int ndp, double angle, char *sign, int ihmsf[4]);
int eraAf2a(char s, int ideg, int iamin, double asec, double *rad);
double eraAnp(double a);
double eraAnpm(double a);
void eraD2tf(int ndp, double days, char *sign, int ihmsf[4]);
int eraTf2a(char s, int ihour, int imin, double sec, double *rad);
int eraTf2d(char s, int ihour, int imin, double sec, double *days);

/* VectorMatrix/BuildRotations */
void eraRx(double phi, double r[3][3]);
void eraRy(double theta, double r[3][3]);
void eraRz(double psi, double r[3][3]);

/* VectorMatrix/CopyExtendExtract */
void eraCp(double p[3], double c[3]);
void eraCpv(double pv[2][3], double c[2][3]);
void eraCr(double r[3][3], double c[3][3]);
void eraP2pv(double p[3], double pv[2][3]);
void eraPv2p(double pv[2][3], double p[3]);

/* VectorMatrix/Initialization */
void eraIr(double r[3][3]);
void eraZp(double p[3]);
void eraZpv(double pv[2][3]);
void eraZr(double r[3][3]);

/* VectorMatrix/MatrixOps */
void eraRxr(double a[3][3], double b[3][3], double atb[3][3]);
void eraTr(double r[3][3], double rt[3][3]);

/* VectorMatrix/MatrixVectorProducts */
void eraRxp(double r[3][3], double p[3], double rp[3]);
void eraRxpv(double r[3][3], double pv[2][3], double rpv[2][3]);
void eraTrxp(double r[3][3], double p[3], double trp[3]);
void eraTrxpv(double r[3][3], double pv[2][3], double trpv[2][3]);

/* VectorMatrix/RotationVectors */
void eraRm2v(double r[3][3], double w[3]);
void eraRv2m(double w[3], double r[3][3]);

/* VectorMatrix/SeparationAndAngle */
double eraPap(double a[3], double b[3]);
double eraPas(double al, double ap, double bl, double bp);
double eraSepp(double a[3], double b[3]);
double eraSeps(double al, double ap, double bl, double bp);

/* VectorMatrix/SphericalCartesian */
void eraC2s(double p[3], double *theta, double *phi);
void eraP2s(double p[3], double *theta, double *phi, double *r);
void eraPv2s(double pv[2][3],
             double *theta, double *phi, double *r,
             double *td, double *pd, double *rd);
void eraS2c(double theta, double phi, double c[3]);
void eraS2p(double theta, double phi, double r, double p[3]);
void eraS2pv(double theta, double phi, double r,
             double td, double pd, double rd,
             double pv[2][3]);

/* VectorMatrix/VectorOps */
double eraPdp(double a[3], double b[3]);
double eraPm(double p[3]);
void eraPmp(double a[3], double b[3], double amb[3]);
void eraPn(double p[3], double *r, double u[3]);
void eraPpp(double a[3], double b[3], double apb[3]);
void eraPpsp(double a[3], double s, double b[3], double apsb[3]);
void eraPvdpv(double a[2][3], double b[2][3], double adb[2]);
void eraPvm(double pv[2][3], double *r, double *s);
void eraPvmpv(double a[2][3], double b[2][3], double amb[2][3]);
void eraPvppv(double a[2][3], double b[2][3], double apb[2][3]);
void eraPvu(double dt, double pv[2][3], double upv[2][3]);
void eraPvup(double dt, double pv[2][3], double p[3]);
void eraPvxpv(double a[2][3], double b[2][3], double axb[2][3]);
void eraPxp(double a[3], double b[3], double axb[3]);
void eraS2xpv(double s1, double s2, double pv[2][3], double spv[2][3]);
void eraSxp(double s, double p[3], double sp[3]);
void eraSxpv(double s, double pv[2][3], double spv[2][3]);

#ifdef __cplusplus
}
#endif
#endif

#ifndef _ERFA_EXTRA_H
#define _ERFA_EXTRA_H

#ifdef __cplusplus
extern "C" {
#endif


  /* MLR 2023-0107
     Since we're short-circuiting the build process, define the package version here.
  */
#define PACKAGE_VERSION "2.0.0"
#define PACKAGE_VERSION_MAJOR 2
#define PACKAGE_VERSION_MINOR 0
#define PACKAGE_VERSION_MICRO 0
#define SOFA_VERSION "20210512"
/* #define DBL_EPSILON */
#ifdef __cplusplus
#include <cfloat>
#else
#include "float.h"
#endif

/*
** Returns the package version
** as defined in configure.ac
** in string format
*/
const char* eraVersion(void);

/*
** Returns the package major version
** as defined in configure.ac
** as integer
*/
int eraVersionMajor(void);

/*
** Returns the package minor version
** as defined in configure.ac
** as integer
*/
int eraVersionMinor(void);

/*
** Returns the package micro version
** as defined in configure.ac
** as integer
*/
int eraVersionMicro(void);

/*
** Returns the orresponding SOFA version
** as defined in configure.ac
** in string format
*/
const char* eraSofaVersion(void);


/*
** Get and set leap seconds (not supported by SOFA; EXPERIMENTAL)
*/

typedef struct {
   int iyear, month;
   double delat;
} eraLEAPSECOND;

int eraGetLeapSeconds(eraLEAPSECOND **leapseconds);
void eraSetLeapSeconds(eraLEAPSECOND *leapseconds, int count);

#ifdef __cplusplus
}
#endif
#endif

#ifndef _ERFA_DAT_EXTRA_H
#define _ERFA_DAT_EXTRA_H

#ifdef __cplusplus
extern "C" {
#endif
  

/*
**  Get the leap second table, initializing it to the built-in version
**  if necessary.
**
**  This function is for internal use in dat.c only and should
**  not be used elsewhere.
*/
int eraDatini(const eraLEAPSECOND *builtin, int n_builtin,
              eraLEAPSECOND **leapseconds);


#ifdef __cplusplus
}
#endif
  
#endif
