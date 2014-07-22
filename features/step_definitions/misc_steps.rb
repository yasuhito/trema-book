# encoding: utf-8
#
# Copyright (C) 2008-2013 NEC Corporation
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2, as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

When(/^wait until "([^"]*)" is up$/) do |process|
  nloop = 0
  pid_file = File.join(Trema.pid, "#{ process }.pid")
  loop do
    nloop += 1
    fail 'Timeout' if nloop > 100 # FIXME
    break if FileTest.exists?(pid_file) && ps_entry_of(process)
    sleep 0.1
  end
  sleep 1  # FIXME
end
