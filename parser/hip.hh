
/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef HIP_HH
#define HIP_HH

#include <functional>
#include <string>
#include <tuple>

namespace HIP
{
struct InvalidLine :
    public std::exception
{
};

struct Parser
{
    typedef std::function<bool (int, char, std::string, std::string, double, short, char, double, double, char, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, int, double, double, double, double, double, char, double, double, char, double, double, char, char, double, double, double, int, char, double, double, double, char, char, char, std::string, char, short, short, char, char, char, std::string, int, double, double, double, double, char, char, char, int, std::string, std::string, std::string, double, std::string, char)> appender_func;
    void operator()(const std::string& text, appender_func func);

    typedef std::tuple<int, char, std::string, std::string, double, short, char, double, double, char, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, int, double, double, double, double, double, char, double, double, char, double, double, char, char, double, double, double, int, char, double, double, double, char, char, char, std::string, char, short, short, char, char, char, std::string, int, double, double, double, double, char, char, char, int, std::string, std::string, std::string, double, std::string, char> data;
    typedef std::function<bool (const data&)> appender_tuple_func;
    void operator()(const std::string& text, appender_tuple_func func);
};

enum fields
{
    HIP,
    PROXY,
    RAHMS,
    DEDMS,
    VMAG,
    VARFLAG,
    R_VMAG,
    RADEG,
    DEDEG,
    ASTROREF,
    PLX,
    PMRA,
    PMDE,
    E_RADEG,
    E_DEDEG,
    E_PLX,
    E_PMRA,
    E_PMDE,
    DE_RA,
    PLX_RA,
    PLX_DE,
    PMRA_RA,
    PMRA_DE,
    PMRA_PLX,
    PMDE_RA,
    PMDE_DE,
    PMDE_PLX,
    PMDE_PMRA,
    F1,
    F2,
    BTMAG,
    E_BTMAG,
    VTMAG,
    E_VTMAG,
    M_BTMAG,
    B_V,
    E_B_V,
    R_B_V,
    V_I,
    E_V_I,
    R_V_I,
    COMBMAG,
    HPMAG,
    E_HPMAG,
    HPSCAT,
    O_HPMAG,
    M_HPMAG,
    HPMAX,
    HPMIN,
    PERIOD,
    HVARTYPE,
    MOREVAR,
    MOREPHOTO,
    CCDM,
    N_CCDM,
    NSYS,
    NCOMP,
    MULTFLAG,
    SOURCE,
    QUAL,
    M_HIP,
    THETA,
    RHO,
    E_RHO,
    DHP,
    E_DHP,
    SURVEY,
    CHART,
    NOTES,
    HD,
    BD,
    COD,
    CPD,
    V_I_RED,
    SPTYPE,
    R_SPTYPE
};

const char field_descriptions[][42] = {
    "Identifier (HIP number)",
    "[HT] Proximity flag",
    "Right ascension in h m s, ICRS (J1991.25)",
    "Declination in deg ' , ICRS (J1991.25)",
    "? Magnitude in Johnson V",
    "[1,3]? Coarse variability flag",
    "[GHT] Source of magnitude",
    "? alpha, degrees (ICRS, Epoch=J1991.25)",
    "? delta, degrees (ICRS, Epoch=J1991.25)",
    "[*+A-Z] Reference flag for astrometry",
    "? Trigonometric parallax",
    "? Proper motion mu_alpha.cos(delta), ICRS",
    "? Proper motion mu_delta, ICRS",
    "? Standard error in RA*cos(DEdeg)",
    "? Standard error in DE",
    "? Standard error in Plx",
    "? Standard error in pmRA",
    "? Standard error in pmDE",
    "[-1/1]? Correlation, DE/RA*cos(delta)",
    "[-1/1]? Correlation, Plx/RA*cos(delta)",
    "[-1/1]? Correlation, Plx/DE",
    "[-1/1]? Correlation, pmRA/RA*cos(delta)",
    "[-1/1]? Correlation, pmRA/DE",
    "[-1/1]? Correlation, pmRA/Plx",
    "[-1/1]? Correlation, pmDE/RA*cos(delta)",
    "[-1/1]? Correlation, pmDE/DE",
    "[-1/1]? Correlation, pmDE/Plx",
    "[-1/1]? Correlation, pmDE/pmRA",
    "? Percentage of rejected data",
    "? Goodness-of-fit parameter",
    "? Mean BT magnitude",
    "? Standard error on BTmag",
    "? Mean VT magnitude",
    "? Standard error on VTmag",
    "[A-Z*-] Reference flag for BT and VTmag",
    "? Johnson B-V colour",
    "? Standard error on B-V",
    "[GT] Source of B-V from Ground or Tycho",
    "? Colour index in Cousins' system",
    "? Standard error on V-I",
    "[A-T] Source of V-I",
    "[*] Flag for combined Vmag, B-V, V-I",
    "? Median magnitude in Hipparcos system",
    "? Standard error on Hpmag",
    "? Scatter on Hpmag",
    "? Number of observations for Hpmag",
    "[A-Z*-] Reference flag for Hpmag",
    "? Hpmag at maximum (5th percentile)",
    "? Hpmag at minimum (95th percentile)",
    "? Variability period (days)",
    "[CDMPRU]? variability type",
    "[12] Additional data about variability",
    "[ABC] Light curve Annex",
    "CCDM identifier",
    "[HIM] Historical status flag",
    "? Number of entries with same CCDM",
    "? Number of components in this entry",
    "[CGOVX] Double/Multiple Systems flag",
    "[PFILS] Astrometric source flag",
    "[ABCDS] Solution quality",
    "Component identifiers",
    "? Position angle between components",
    "? Angular separation between components",
    "? Standard error on rho",
    "? Magnitude difference of components",
    "? Standard error on dHp",
    "[S] Flag indicating a Survey Star",
    "[DG] Identification Chart",
    "[DGPWXYZ] Existence of notes",
    "[1/359083]? HD number <III/135>",
    "Bonner DM <I/119>, <I/122>",
    "Cordoba Durchmusterung (DM) <I/114>",
    "Cape Photographic DM <I/108>",
    "V-I used for reductions",
    "Spectral type",
    "[1234GKSX]? Source of spectral type"
};
}
#endif // HIP_HH
