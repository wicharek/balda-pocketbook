# coding: utf-8

require 'net/http'
require 'nokogiri'

initial_path='/w/index.php?title=%D0%9A%D0%B0%D1%82%D0%B5%D0%B3%D0%BE%D1%80%D0%B8%D1%8F:%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B5_%D1%81%D1%83%D1%89%D0%B5%D1%81%D1%82%D0%B2%D0%B8%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D1%8B%D0%B5&from=%E0%E0%EA'
path = initial_path

page_num = 0
word_num = 0

puts "Started"
f = File.open('words.txt', 'w')

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
