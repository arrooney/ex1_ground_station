/*
 * Copyright 2015 Brendan Bruner
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
 * Jun 24, 2016
 */
#ifndef CLASS_CERROR_H_
#define CLASS_CERROR_H_

typedef enum
{
	COBJ_OK = 0,
	COBJ_ALLOC_FAIL = 1,
	COBJ_INV_PARAM = 2,
	COBJ_EXT_FAIL = 3
} CError;

#endif /* CLASS_CERROR_H_ */
