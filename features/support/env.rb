# encoding: utf-8

require 'aruba/cucumber'
require 'trema'

ENV['TREMA_TMP'] = File.join(__dir__, '..', '..', 'tmp', 'aruba')

def ps_entry_of(name)
  `ps -ef | grep -w "#{name} " | grep -v grep`
end
