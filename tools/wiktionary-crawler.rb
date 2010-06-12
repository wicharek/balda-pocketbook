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

require 'net/http'
require 'nokogiri'

initial_path='/w/index.php?title=%D0%9A%D0%B0%D1%82%D0%B5%D0%B3%D0%BE%D1%80%D0%B8%D1%8F:%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B5_%D1%81%D1%83%D1%89%D0%B5%D1%81%D1%82%D0%B2%D0%B8%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D1%8B%D0%B5&from=%E0%E0%EA'
path = initial_path

page_num = 0
word_num = 0

puts "Started"
f = File.open('words_raw.txt', 'w')

while path
	res = Net::HTTP.start('ru.wiktionary.org', 80) {|http|
		http.request(Net::HTTP::Get.new(path))
	}

	doc = Nokogiri::HTML(res.body)
	doc.css('div#mw-pages > table > tr > td > ul > li > a').each do |a_word|
		word = a_word['title']
		
		if !word.include? '-' then # exclude words with dash
			f.write("#{word}\n")
			word_num += 1
		end
	end

	# Find next page link
	path = nil
	(doc / 'a').each do |a|
		if a.text == 'следующие 200' then
			path = a['href']
			break
		end
	end
	
	page_num += 1
	puts "Page ##{page_num} parsed"
	sleep(0.5) # Be gentle, wait for some time before next request. Not much needed actually. Our requests are not that heavy.
end

puts "Ready: #{word_num} words"

f.close
