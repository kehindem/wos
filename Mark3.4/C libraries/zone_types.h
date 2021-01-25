/************************************************************************
 *  @filename: Zone Types
 * 	@description: Enumerations for zones and zone status
 *  @author Michael Kehinde <kehindem@mcmaster.ca>
 *  @date October 14, 2020
 ***********************************************************************/

#ifndef ZONE_TYPES_H
#define ZONE_TYPES_H

typedef enum 
{
	FRONT,
	BACK
} zone_t;

typedef enum 
{
	VACANT,
	OCCUPIED
} zone_status_t;

#endif 
