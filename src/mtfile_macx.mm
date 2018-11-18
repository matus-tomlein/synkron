/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2009 Matus Tomlein (matus.tomlein@gmail.com)

 Synkron is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public Licence
 as published by the Free Software Foundation; either version 2
 of the Licence, or (at your option) any later version.

 Synkron is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public Licence for more details.

 You should have received a copy of the GNU General Public Licence
 along with Synkron; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
********************************************************************/

#include "mtfile.h"

#ifdef Q_WS_MAC
#ifndef USE_UNIX_TOUCH_COMMAND

#import <Foundation/NSFileManager.h>
#import <Foundation/NSString.h>

bool MTFile::copy(QString dest)
{
	const char * str1 = QByteArray().append(dest).constData();
	NSString * nsdest = [[NSString alloc] initWithUTF8String:str1];
	QFileInfo srcinfo(*this);
	const char * str2 = QByteArray().append(srcinfo.absoluteFilePath()).constData();
	NSString * nssrc = [[NSString alloc] initWithUTF8String:str2];
	NSFileManager * fileManager = [NSFileManager defaultManager];
	if ([fileManager fileExistsAtPath:nssrc]) {
		if ([fileManager copyPath:nssrc toPath:nsdest handler:nil] == YES) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

#endif
#endif
