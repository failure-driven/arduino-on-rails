class CreateCounters < ActiveRecord::Migration[6.0]
  def change
    create_table :counters do |t|
      t.string :name, null: false, index: { unique: true }
      t.bigint :count, null: false, default: 0

      t.timestamps
    end
  end
end
