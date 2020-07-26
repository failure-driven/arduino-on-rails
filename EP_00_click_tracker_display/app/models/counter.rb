class Counter < ApplicationRecord
  def self.increment_default_counter
    Counter
      .find_or_create_by(name: 'default')
      .increment!(:count)
  end
end
