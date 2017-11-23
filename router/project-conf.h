#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

//#define RPL_CONF_LEAF_ONLY 1	// set 1 to set END DEVICE

#ifndef WITH_NON_STORING
#define WITH_NON_STORING 1 /* Set this to run with non-storing mode */
#endif /* WITH_NON_STORING */

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#undef UIP_CONF_MAX_ROUTES

#ifdef TEST_MORE_ROUTES
/* configure number of neighbors and routes */
#define NBR_TABLE_CONF_MAX_NEIGHBORS     10
#define UIP_CONF_MAX_ROUTES   30
#else
/* configure number of neighbors and routes */
#define NBR_TABLE_CONF_MAX_NEIGHBORS     10
#define UIP_CONF_MAX_ROUTES   10
#endif /* TEST_MORE_ROUTES */

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC    nullrdc_driver
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK       1

/* Define as minutes */
#define RPL_CONF_DEFAULT_LIFETIME_UNIT   10//60

/* 10 minutes lifetime of routes */
#define RPL_CONF_DEFAULT_LIFETIME        10//10

#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME 1//1

#if WITH_NON_STORING
#undef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM 40 /* Number of links maintained at the root. Can be set to 0 at non-root nodes. */
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 0 /* No need for routes */
#undef RPL_CONF_MOP
#define RPL_CONF_MOP RPL_MOP_NON_STORING /* Mode of operation*/
#endif /* WITH_NON_STORING */

/* son define */
#ifndef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER  1
#endif

#define UIP_CONF_IPV6_RPL  1
/* ND and Routing */
#define UIP_CONF_ND6_SEND_RA        0
#define UIP_CONF_IP_FORWARD         0

#define UIP_CONF_DS6_NBR_NBU				15

#define UIP_CONF_DS6_ROUTE_NBU				15

#endif
