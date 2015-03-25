require 'trema'
require 'sliceable_switch'

describe SliceableSwitch do
  Given { allow(Trema::Logger).to receive(:new).and_return(spy('logger')) }
  Given(:sliceable_switch) { SliceableSwitch.new.tap(&:start) }

  describe '#slice_list' do
    When(:initial_slices) { sliceable_switch.slice_list }
    Then { initial_slices == [] }
  end

  describe '#find_slice' do
    context "with 'foo'" do
      When(:result) { sliceable_switch.find_slice('foo') }
      Then { result == Failure(SliceableSwitch::SliceNotFoundError) }
    end
  end

  describe '#add_slice' do
    context "with 'foo'" do
      When { sliceable_switch.add_slice 'foo' }
      Then { sliceable_switch.slice_list == ['foo'] }

      describe '#find_slice' do
        context "with 'foo'" do
          When(:initial_slice) { sliceable_switch.find_slice 'foo' }
          Then { initial_slice.empty? }
        end
      end

      describe '#add_slice' do
        context "with 'foo'" do
          When(:result) { sliceable_switch.add_slice 'foo' }
          Then { result == Failure(SliceableSwitch::SliceAlreadyExistsError) }
        end
      end
    end
  end

  describe '#delete_slice' do
    context "with 'foo'" do
      When(:result) { sliceable_switch.delete_slice 'foo' }
      Then { result == Failure(SliceableSwitch::SliceNotFoundError) }
    end
  end
end
