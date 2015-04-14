/*
 Flexisip, a flexible SIP proxy server with media capabilities.
 Copyright (C) 2012  Belledonne Communications SARL.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as
 published by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef expressionparser_hh
#define expressionparser_hh

#ifndef NO_SOFIA
#include "sofia-sip/sip.h"
#endif

#include <string>
#include <memory>
#include <functional>

class SipAttributes;

void log_boolean_expression_evaluation(bool value);
void log_boolean_expression_parsing(bool value);


class BooleanExpression{
protected:
	BooleanExpression(){}
	typedef std::function<bool(const SipAttributes* )> GeneratorFun_t;
public:
#ifndef NO_SOFIA
		bool eval(const sip_t *sip);
#endif
		virtual ~BooleanExpression();
		virtual GeneratorFun_t getGenerator()=0;
		static std::shared_ptr<BooleanExpression> parse(const std::string &str);
		long ptr();
		bool eval(const SipAttributes *args);
};



#endif
