require 'fdb'

describe FDB::Entry do
  describe '.new' do
    context 'with port_no = 1' do
      Given(:entry) { FDB::Entry.new('11:22:33:44:55:66', 1, 1) }

      describe '#update' do
        context 'with 2' do
          When { entry.update(2) }
          Then { entry.port_no == 2 }
        end

        context 'with 1' do
          When { entry.update(1) }
          Then { entry.port_no == 1 }
        end
      end
    end

    context 'with age_max = 0' do
      Given(:entry) { FDB::Entry.new('11:22:33:44:55:66', 1, 0) }
      Then { entry.aged_out? == true }
    end

    context 'when age_max = 10' do
      Given(:entry) { FDB::Entry.new('11:22:33:44:55:66', 1, 10) }
      Then { entry.aged_out? == false }
    end
  end
end
