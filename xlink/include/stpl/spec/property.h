/******************************************************************************
 * This file is part of the Simple Text Processing Library(STPL).
 * (c) Copyright 2007-2008 TYONLINE TECHNOLOGY PTY. LTD.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU LESSER GENERAL PUBLIC LICENSE, Version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *******************************************************************************
 *
 * @author				Ling-Xiang(Eric) Tang
 *
 *******************************************************************************/

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include "../stpl/stpl_property.h"
#include "../stpl/stpl_stream.h"
#include "../stpl/stpl_parser.h"

#include <string>

namespace stpl {
	typedef Document<Property<> > 						Properties;
	typedef Properties::entity_type 					PropertyEntityType;
	typedef Scanner<PropertyEntityType> 				PropertiesScanner;
	typedef GeneralGrammar<Properties > 				PropertiesGrammar;

	typedef Parser<PropertiesGrammar
					, Properties
					, PropertyEntityType
					, PropertiesScanner
					> 									PropertiesParser;

	typedef Document<Property<std::string, char *> >	PropertyDocument;
	typedef FileStream<PropertyDocument>				PropertiesFile;
	typedef PropertiesFile::entity_type 				PropertiesFileEntityType;
	typedef Scanner<PropertiesFileEntityType> 			PropertiesFileScanner;
	typedef GeneralGrammar<PropertiesFile> 				PropertiesFileGrammar;

	typedef Parser<PropertiesFileGrammar
					, PropertiesFile
					, PropertiesFileEntityType
					, PropertiesFileScanner
					> 									PropertiesFileParser;
}
#endif /* PROPERTY_H_ */
