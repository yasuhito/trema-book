# encoding: utf-8

require 'fdb'

describe FDB do
  Given(:fdb) { FDB.new }

  describe '#learn' do
    context "with MAC='11:22:33:44:55:66', port_no=1" do
      When { fdb.learn('11:22:33:44:55:66', 1) }

      Then { fdb.lookup('11:22:33:44:55:66') == [nil, 1] }
      Then { fdb.port_no_of('11:22:33:44:55:66') == 1 }
    end
  end
end
