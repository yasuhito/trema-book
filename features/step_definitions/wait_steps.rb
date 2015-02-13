When(/^I wait until a file named "(.*?)" is created$/) do |file|
  Timeout.timeout(exit_timeout) do
    in_current_dir do
      loop do
        break if FileTest.exists?(file)
        sleep 0.1
      end
    end
  end
end
