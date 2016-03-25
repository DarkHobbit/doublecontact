/* Double Contact
 *
 * Module: Global definitions
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

// Visible columns
enum ContactColumn {
  ccFirstName, ccSecondName, ccFullName, // names
  ccPhone, // first or preferred phone
  ccEMail, // first or preferred email
  ccLast
};

// Visible columns headers
extern const QString contactColumnHeaders[ccLast];


#endif // GLOBALS_H
