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
# Builds a balda_dict_data.c file from words.txt
# This script expect that input file contains only valid words and
# is sorted ascending
#

class BaldaDataTreeEntry
	attr_accessor :char, :children, :ends_here
	
	def initialize(char, ends_here=false)
		@char = char
		@children = Hash.new
		@ends_here = ends_here
	end
	
	def append(char, ends_here)
		unless @children.has_key?(char) then
			return @children[char] = BaldaDataTreeEntry.new(char, ends_here)
		else
			if ends_here then
				@children[char].ends_here = true
			end
			
			return @children[char]
		end
	end
	
	def single?
		return (@children.length == 1 and not @ends_here)
	end
	
	def print_debug(out, offset='', needs_offset=true)
		out.print (needs_offset ? offset : ''), char, (ends_here ? '!' : ''),
			((!single? or @children.length == 0) ? "\n" : '')
		
		#out.print "[#{@children.length}]: \n"
		@children.values.each { |entry|
			entry.print_debug(out, offset + (@char ? ' ' : ''),
				(!single? or @children.length == 0))
		}
	end
end

class BaldaDataChunkEntry
	attr_accessor :char, :ends_here, :next_chunk
	
	def initialize(char, ends_here, next_chunk)
		@char = char
		@ends_here = ends_here
		@next_chunk = next_chunk
	end
end

class BaldaDataChunk
	LIST=1
	PLAIN=2
	
	attr_accessor :type, :offset
	
	def initialize(type)
		@type = type
		@offset = 0
	end
end

class BaldaListDataChunk<BaldaDataChunk
	attr_accessor :list
	
	def initialize()
		super(BaldaDataChunk::LIST)
		@list = Array.new
	end
	
	def size_dwords()
		return 1 + ((@list.length == 1) ? 0 : @list.length)
	end
end

class BaldaPlainDataChunk<BaldaDataChunk
	attr_accessor :string, :next_chunk, :ends_here

	def initialize(string='')
		super(BaldaDataChunk::PLAIN)
		@string = string
	end
	
	def size_dwords()
		size_bits = 8 + string.length * 5 + (next_chunk.nil? ? 0 : 24)
		size_bytes = size_bits / 8 + ((size_bits % 8 != 0) ? 1 : 0)
		return size_bytes / 4 + ((size_bytes % 4 != 0) ? 1 : 0)
	end
end

class BaldaData
	attr_accessor :first_chunk, :all_chunks, :data_size_dwords, :dwords_written
	
	# Create data from tree.
	def initialize(tree_root)
		@stats_plain_strings_c = Hash.new
		@stats_plain_strings = Hash.new
		@dwords_written = 0
		
		@char_table = Hash.new
		@char_table['а'] = 0;
		@char_table['б'] = 1;
		@char_table['в'] = 2;
		@char_table['г'] = 3;
		@char_table['д'] = 4;
		@char_table['е'] = 5;
		@char_table['ж'] = 6;
		@char_table['з'] = 7;
		@char_table['и'] = 8;
		@char_table['й'] = 9;
		@char_table['к'] = 10;
		@char_table['л'] = 11;
		@char_table['м'] = 12;
		@char_table['н'] = 13;
		@char_table['о'] = 14;
		@char_table['п'] = 15;
		@char_table['р'] = 16;
		@char_table['с'] = 17;
		@char_table['т'] = 18;
		@char_table['у'] = 19;
		@char_table['ф'] = 20;
		@char_table['х'] = 21;
		@char_table['ц'] = 22;
		@char_table['ч'] = 23;
		@char_table['ш'] = 24;
		@char_table['щ'] = 25;
		@char_table['ъ'] = 26;
		@char_table['ы'] = 27;
		@char_table['ь'] = 28;
		@char_table['э'] = 29;
		@char_table['ю'] = 30;
		@char_table['я'] = 31;
	
		@first_chunk = get_chunk(tree_root)
		self.flatten
		self.calc_offsets
	end
	
	def get_chunk(tree_entry)
		return nil if tree_entry.children.empty?
	
		if tree_entry.single? then
			if tree_entry.children.values.first.children.length > 1 then
				# more efficient is to use list with single element here
				chunk = BaldaListDataChunk.new
				
				e = tree_entry.children.values.first
				chunk.list.push BaldaDataChunkEntry.new(e.char, e.ends_here, self.get_chunk(e))
			else
				# gather string
				chunk = BaldaPlainDataChunk.new
				
				while tree_entry.single?
					tree_entry = tree_entry.children.values.first
					chunk.string << tree_entry.char
				end
				
				if tree_entry.ends_here then
					chunk.ends_here = true
				end
				
				chunk.next_chunk = get_chunk(tree_entry)
				
				prev_c = @stats_plain_strings_c[chunk.string.length]
				@stats_plain_strings_c[chunk.string.length] = prev_c+1 unless prev_c.nil?
				@stats_plain_strings_c[chunk.string.length] = 1 if prev_c.nil?
				
				prev_c = @stats_plain_strings[chunk.string]
				@stats_plain_strings[chunk.string] = prev_c+1 unless prev_c.nil?
				@stats_plain_strings[chunk.string] = 1 if prev_c.nil?
			end
			
			return chunk
		else
			# make a list
			chunk = BaldaListDataChunk.new
			
			tree_entry.children.values.sort{ |a, b| a.char <=> b.char }.each { |e|
				chunk.list.push BaldaDataChunkEntry.new(e.char, e.ends_here, self.get_chunk(e))
			}
			
			return chunk
		end
	end
	
	def flatten()
		@all_chunks = Array.new
		@data_size_dwords = 0
		flatten_callback(@first_chunk)
	end
	
	def flatten_callback(chunk)
		return if chunk.nil?
		
		@all_chunks.push(chunk)
		
		if chunk.type == BaldaDataChunk::LIST then
			chunk.list.each do |e|
				flatten_callback(e.next_chunk)
			end
		else
			flatten_callback(chunk.next_chunk)
		end
	end
	
	def calc_offsets()
		off = 1 # start from 1, 0 is reserved for invalid/empty offset
		@all_chunks.each do |chunk|
			chunk.offset = off
			@data_size_dwords += chunk.size_dwords
			off += chunk.size_dwords
		end
	end
	
	def print_debug(out)
		out.puts 'Chunks:'
		out.puts ''
		@all_chunks.each do |chunk|
			#if chunk.nil? then
			#	out.puts 'WTF?'
			#else
			out.puts "CHUNK ##{chunk.offset}"
			out.puts "  type: #{chunk.type == BaldaDataChunk::LIST ? 'LIST' : 'PLAIN'}"
			out.puts "  size: #{chunk.size_dwords}"
			
			if chunk.type == BaldaDataChunk::LIST then
				# header
				out.puts "  list [#{chunk.list.length}]:"
				chunk.list.each do |e|
					out.puts "    '#{e.char}': ##{e.next_chunk.nil? ? 'NULL' : e.next_chunk.offset} #{e.ends_here ? '!' : ''}"
				end
			else
				out.puts "  string[#{chunk.string.length}]: #{chunk.string} #{chunk.ends_here ? '!' : ''}"
				out.puts "  next: ##{chunk.next_chunk.nil? ? 'NULL' : chunk.next_chunk.offset}"
			end
			#end
			
			out.puts ''
		end
		
		out.puts 'Plain string stats (by length):'
		@stats_plain_strings_c.to_a.sort{ |a, b| b[1] <=> a[1] }.each { |c|
			out.puts "string[#{c[0]}]: #{c[1]}"
		}
		out.puts ''
		
		out.puts 'Plain string stats (by string):'
		@stats_plain_strings.to_a.sort{ |a, b| b[1] <=> a[1] }.each { |c|
			out.puts "string[#{c[0]}]: #{c[1]}"
		}
		out.puts ''
	end
	
	HEADER_SINGLE_CHAR_FLAG = 0x20000000
	HEADER_LAST_CHUNK_FLAG = 0x20000000
	HEADER_ENDS_HERE_FLAG = 0x40000000
	HEADER_PLAIN_FLAG = 0x80000000
	HEADER_COUNT_MASK = 0x1F000000
	
	ENTRY_ENDS_HERE_FLAG = 0x80000000
	
	def encode_char(c)
		return @char_table[c]
	end
	
	def write_as_c_uint_array(out, str_inc="\t")
		@c_uint_str_inc = str_inc
		@c_uint_first = true
		@c_uint_out = out
		
		out.puts "#{str_inc}0x00000000, // reserved for empty/invalid offset"
		
		@all_chunks.each do |chunk|
			if chunk.type == BaldaDataChunk::LIST then
				# header
				if chunk.list.length == 1 then
					# Single entry
					write_c_uint_array_value(
						HEADER_SINGLE_CHAR_FLAG |
						(chunk.list.first.ends_here ? HEADER_ENDS_HERE_FLAG : 0) |
						(self.encode_char(chunk.list.first.char) << 24) | # char
						(chunk.list.first.next_chunk.nil? ? 0 : (chunk.list.first.next_chunk.offset & 0xffffff)) # offset
					)
				else
					# Multiple entries
					write_c_uint_array_value(chunk.list.length << 24)
					
					chunk.list.each do |e|
						write_c_uint_array_value(
							(e.ends_here ? ENTRY_ENDS_HERE_FLAG : 0) |
							(self.encode_char(e.char) << 24) | # char
							(e.next_chunk.nil? ? 0 : (e.next_chunk.offset & 0xffffff)) # offset
						)
					end
				end
			else # if BaldaDataChunk::PLAIN
				chars = Array.new
				chunk.string.each_char { |c|
					chars.push self.encode_char(c)
				}
				
				if chunk.next_chunk.nil?
					# optimized
					self.write_c_uint_string(chars, 
						HEADER_PLAIN_FLAG | HEADER_LAST_CHUNK_FLAG |
							((chars.length << 24) & HEADER_COUNT_MASK) |
							(chunk.ends_here ? HEADER_ENDS_HERE_FLAG : 0), 8)
				else
					# normal, has next chunk
					# header
					write_c_uint_array_value(
						HEADER_PLAIN_FLAG |
						(chunk.ends_here ? HEADER_ENDS_HERE_FLAG : 0) |
						((chars.length << 24) & HEADER_COUNT_MASK) |
						(chunk.next_chunk.offset & 0xffffff) # offset
					)
					
					#data
					self.write_c_uint_string(chars, 0, 0)
				end
			end
		end
		
		out.puts ''
	end
	
	def write_c_uint_string(chars, value, offset)
		chars.each { |c|
			shift = 32-offset-5
			#if shift > 0 then
				value |= ((c & 0x1F) << shift)
			#else
			#	value |= ((c & 0x1F) >> -shift)
			#end
			
			if shift <= 0 then
				self.write_c_uint_array_value(value)
				value = 0
				
				if shift < 0 then
					value |= (((c & 0x1F) << (32+shift)) & 0xffffffff)
				end
			end
			
			offset = (offset + 5) % 32
		}
		
		# not written data left
		self.write_c_uint_array_value(value) if (offset != 0)
	end
	
	def write_c_uint_array_value(value)
		@dwords_written += 1
		
		if @c_uint_first then
			@c_uint_col = 0
			@c_uint_first = false
			@c_uint_out.print @c_uint_str_inc
		else
			@c_uint_out.print ', '
			
			if @c_uint_col == 6 then
				@c_uint_col = 0
				@c_uint_out.print "\n#{@c_uint_str_inc}"
			end
		end
		
		@c_uint_out.print "0x#{format('%08x', value)}"
		@c_uint_col = @c_uint_col + 1
	end
end


Encoding.default_external = Encoding::UTF_8
Encoding.default_internal = Encoding::UTF_8

root_entry = BaldaDataTreeEntry.new(nil)

File.readlines('words.txt').each { |l|
	unless l.empty? then
		prev_entry = root_entry
		index = 0
		l.each_char { |c|
			if c != "\n" then
				prev_entry = prev_entry.append(c, index == (l.length - 2))
			end
			index=index+1
		}
	end
}

data = BaldaData.new(root_entry)

puts "Data chunks ready. Total size: #{data.data_size_dwords * 4} bytes"

#root_entry.print_debug(out_f)
#puts "Writing debug info to 'chunks.txt'..."
#data.print_debug(File.open('chunks.txt', 'w'))

c_data_file_path = 'balda_dict_data.c'

puts "Writing #{c_data_file_path}..."
out_f = File.open(c_data_file_path, 'w')

# Write header
out_f.puts'/***************************************************************************'
out_f.puts' *   This file is a part of Balda game for PocketBook.                     *'
out_f.puts' *   Copyright 2010 by Vitaliy Ivanov <wicharek@w2f2.com>                  *'
out_f.puts' *                                                                         *'
out_f.puts' *   This program is free software; you can redistribute it and/or modify  *'
out_f.puts' *   it under the terms of the GNU General Public License as published by  *'
out_f.puts' *   the Free Software Foundation; either version 2 of the License, or     *'
out_f.puts' *   (at your option) any later version.                                   *'
out_f.puts' *                                                                         *'
out_f.puts' *   This program is distributed in the hope that it will be useful,       *'
out_f.puts' *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *'
out_f.puts' *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *'
out_f.puts' *   GNU General Public License for more details.                          *'
out_f.puts' *                                                                         *'
out_f.puts' *   You should have received a copy of the GNU General Public License     *'
out_f.puts' *   along with this program; if not, write to the                         *'
out_f.puts' *   Free Software Foundation, Inc.,                                       *'
out_f.puts' *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *'
out_f.puts' ***************************************************************************/'
out_f.puts('')
out_f.puts('#include "balda_dict.h"')
out_f.puts('')
out_f.puts('balda_dict_data_t balda_dict_data[] = {')

#Write data
data.write_as_c_uint_array(out_f)

# Write footer
out_f.puts('};')

puts "Done. Bytes written: #{data.dwords_written * 4}"
