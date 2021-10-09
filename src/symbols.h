/*
 * 	symbols.h
 *
 *	Version: 1.0.0
 *  Created on: Dec 14, 2018
 *  Author: Julian Stiefel
 *  License: BSD 3-Clause
 *  Description: File containing the bitmaps for direction arrows.
 */

#ifndef _symbols_H_
#define _symbols_H_

#include <stdio.h>

#define width 64
#define height 64

extern const uint8_t ic_nav_arrow_finish[];
extern const uint8_t ic_nav_arrow_fork_left[];
extern const uint8_t ic_nav_arrow_fork_right[];
extern const uint8_t ic_nav_arrow_goto_start[];
extern const uint8_t ic_nav_arrow_keep_going[];
extern const uint8_t ic_nav_arrow_keep_left[];
extern const uint8_t ic_nav_arrow_keep_right[];
extern const uint8_t ic_nav_arrow_start[];
extern const uint8_t ic_nav_arrow_turn_hard_left[];
extern const uint8_t ic_nav_arrow_turn_hard_right[];
extern const uint8_t ic_nav_arrow_turn_left[];
extern const uint8_t ic_nav_arrow_turn_right[];
extern const uint8_t ic_nav_arrow_uturn[];
extern const uint8_t ic_nav_outof_route[];
extern const uint8_t ic_nav_roundabout_ccw1_1[];
extern const uint8_t ic_nav_roundabout_ccw1_2[];
extern const uint8_t ic_nav_roundabout_ccw1_3[];
extern const uint8_t ic_nav_roundabout_ccw2_2[];
extern const uint8_t ic_nav_roundabout_ccw2_3[];
extern const uint8_t ic_nav_roundabout_cw3_3[];
extern const uint8_t ic_nav_roundabout_exit_ccw[];
extern const uint8_t ic_nav_roundabout_exit_cw[];
extern const uint8_t ic_nav_roundabout_fallback[];
extern const uint8_t ic_nav_roundabout_ccw3_3[] ;
extern const uint8_t ic_nav_roundabout_cw1_1[];
extern const uint8_t ic_nav_roundabout_cw1_2[];
extern const uint8_t ic_nav_roundabout_cw1_3[];
extern const uint8_t ic_nav_roundabout_cw2_2[];
extern const uint8_t ic_nav_roundabout_cw2_3[];
extern const uint8_t not_defined[];
extern const uint8_t ferry_train[];
extern const uint8_t *nav_symbols[];

#endif /* _symbols_H_ */