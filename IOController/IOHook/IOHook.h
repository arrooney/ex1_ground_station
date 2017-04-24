/*
 * Copyright 2017 Brendan Bruner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * bbruner@ualberta.ca
 * April 11, 2017
 */
/**
 * @file
 */

#ifndef IOHOOK_H_
#define IOHOOK_H_

#include <CCThreadedQueue.h>

typedef int (*IOHook_Printf_FP)(const char*, ...);
typedef int (*IOHook_Getchar_FP)( );

extern int IOHook_Init( );

extern IOHook_Printf_FP IOHook_GetPrintf( );
extern IOHook_Getchar_FP IOHook_GetGetchar( );

extern struct CCThreadedQueue* IOHook_GetGomshellOutputQueue( );
extern struct CCThreadedQueue* IOHook_GetGomshellInputQueue( );

#endif /* IOHOOK_H_ */
