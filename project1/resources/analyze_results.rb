#!/usr/bin/env ruby
require 'cmath'
require 'gruff'

logfile  = ARGV[0]
dictfile = ARGV[1]

logfile  ||= 'english.log'
dictfile ||= 'english_words.txt'

# get dict_size_of
def analyze_dict path
  words = File.read(path).split(/\s*\n\s*/)
  Hash[ words.group_by(&:length).map { |k,v| [k, v.length] } ]
end

class Record
  attr_accessor :time, :num_of_word_len, :hardness, :trapdoor_num

  def self.parse line, dict_size_of
    time, plains = line.split ','
    time = time.to_i
    plains = plains.split
    Record.new(time, plains, dict_size_of)
  end

  def initialize time, plains, dict_size_of
    @time = time
    @plains = plains
    compute_hardness dict_size_of
  end

  protected
  def compute_hardness dict_size_of
    @num_of_word_len = Hash.new(0)
    @plains.each do |word|
      @num_of_word_len[word.length] += 1
    end

    @hardness = 1
    num_of_word_len.each do |word_length, num|
      @hardness *= num * (dict_size_of[word_length] ** num)
    end
    @hardness = @hardness.to_s.length

    trapdoors = dict_size_of.keys.select { |len| dict_size_of[len] < 5000 }
    @trapdoor_num = 0
    trapdoors.each { |len|  @trapdoor_num += @num_of_word_len[len] }
  end
end

dict_size_of = analyze_dict(dictfile)
lines = File.read(logfile).split(/\s*\r?\n\s*/)
records = lines.map { |l| Record.parse(l, dict_size_of) }
hardness_to_time = records.map { |r| [r.time, r.num_of_word_len] } .sort_by(&:first)
trapdoors_to_time = records.map { |r| [r.trapdoor_num, r.time] } .sort_by(&:first)

labels = []
word_lengthes = dict_size_of.keys.sort
word_lengthes.each_with_index { |len, i| labels.push [i, len.to_s] }
labels = Hash[labels]

SIZE = 800
g = Gruff::Bar.new(SIZE)
g.hide_legend = true
g.title = 'Frequency of english_words.txt'
g.labels = labels
data = word_lengthes.map { |len| dict_size_of[len] }
g.data(:data, data)
g.write('english_words_freq.png')

g = Gruff::Scatter.new(SIZE)
g.title = 'Running time - "trapdoor" numbers'
trapdoors_to_time.select! { |e| e[0] < 100 }
passed, timeout = trapdoors_to_time.partition { |e| e[1] < 120 }
g.data('passed', passed.map(&:first), passed.map(&:last))
g.data('timeout', timeout.map(&:first), timeout.map(&:last))
g.write('trapdoors_to_success.png')

hardness_to_time.each_with_index do |rec, i|
  time, histo = rec
  g = Gruff::Bar.new(SIZE)
  g.title = time.to_s
  g.hide_legend = true
  g.labels = labels

  data = word_lengthes.map { |len| histo[len] }
  g.data(:data, data)

  puts 'writing charts'
  g.write("hist-#{i}-#{time}.png");
end
