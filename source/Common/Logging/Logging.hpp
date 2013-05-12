/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012-2013 Richard Maxwell <jodi.the.tigger@gmail.com>
    
    This file is part of Game-in-a-box
    
    Game-in-a-box is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.
    
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef ARGH_LOGGING_HPP
#define ARGH_LOGGING_HPP

namespace GameInABox { namespace Common { namespace Logging {

enum class LogLevel
{
    // Debug Spew, doesn't contain any infomation that's useful in release builds.
    // Used for debugging, but too useful to delete the code that makes the spew.
    Debug,

    // Debug spew for release builds. Stuff you can afford to be in log files for
    // Release builds (but not something that would make the logs huge, use debug for that).
    Informational,

    // Hmm, that's interesting. Internal program state like births, deaths or marriages.
    Notice,

    // Program has failed in a recoverable way, keep on trucking...
    Warning,

    // Something you should really take note of, but doesn't invalidate program
    // state in anyway. Like a Warning, but more important. For example, you've
    // Stated running out of memory, but you still have some more up your sleve
    // before you go Critical.
    Alert,

    // Program has failed and will reset to a state as if freshly started.
    Error,

    // Program cannot run, impending quit or segfault.
    Critical
};

// Eh, for some reason I cannot have the body in the header file?
// For some reason the include guards don't work and I get multiple
// defintion warnings.
void Log(LogLevel, ...);

}}};

#endif // ARGH_LOGGING_HPP
