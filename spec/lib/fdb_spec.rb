require 'fdb'

describe FDB do
  Given(:fdb) { FDB.new }

  describe '#lookup' do
    When(:result) { fdb.lookup(mac) }

    context "with '11:22:33:44:55:66'" do
      When(:mac) { '11:22:33:44:55:66' }
      Then { result.nil? }
    end
  end

  describe '#learn' do
    context "with MAC='11:22:33:44:55:66', port_no=1" do
      When { fdb.learn('11:22:33:44:55:66', 1) }
      Then { fdb.lookup('11:22:33:44:55:66') == 1 }

      describe '#learn' do
        context "with MAC='11:22:33:44:55:66', port_no=1" do
          When { fdb.learn('11:22:33:44:55:66', 1) }
          Then { fdb.lookup('11:22:33:44:55:66') == 1 }
        end
      end
    end
  end

  describe '#age' do
    When(:result) { fdb.age }
    Then { result == {} }
  end
end
