# encoding: utf-8

#
#   This file is a part of Balda game for PocketBook.
#   Copyright 2010 by Vitaliy Ivanov <wicharek@w2f2.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .
#

#
# Process text file dictionary (replace ё with е, remove words that
# start from capital letter) and sort words.
#

Encoding.default_external = Encoding::UTF_8
Encoding.default_internal = Encoding::UTF_8

a = Array.new
f = File.open('words_sorted.txt', 'w')
File.readlines('words.txt').each { |l| a.push(l.sub('ё', 'е')) }
a.sort.each { |l|
	# Save only words that contain lowercase Russian letter characters
	if l =~ /^[а-я]+$/ then
		f.puts(l)
	end
}
