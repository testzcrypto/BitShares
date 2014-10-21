#pragma once

#include <bts/wallet/wallet_records.hpp>

namespace bts { namespace wallet {

   namespace detail { class wallet_db_impl; }

   class wallet_db
   {
      public:
         wallet_db();
         ~wallet_db();

         void open( const fc::path& wallet_file );
         void close();
         bool is_open()const;

         int32_t new_wallet_record_index();

         void        set_property( property_enum property_id, const fc::variant& v );
         fc::variant get_property( property_enum property_id )const;

         // Wallet child keys
         uint32_t           get_last_wallet_child_key_index()const;
         void               set_last_wallet_child_key_index( uint32_t key_index );
         private_key_type   get_wallet_child_key( const fc::sha512& password, uint32_t key_index )const;
         public_key_type    generate_new_account( const fc::sha512& password, const string& account_name, const variant& private_data );

         // Account child keys
         private_key_type   get_account_child_key( const fc::sha512& password, const address& account_address, uint32_t seq_num )const;
         private_key_type   generate_new_account_child_key( const fc::sha512& password, const string& account_name );

         void               add_contact_account( const account_record& blockchain_account_record, const variant& private_data );

         void store_key( const key_data& k );
         void store_transaction( wallet_transaction_record& t );
         void cache_balance( const bts::blockchain::balance_record& b );
         void store_account( wallet_account_record& );
         void cache_memo( const memo_status& memo,
                          const private_key_type& account_key,
                          const fc::sha512& password );

         void remove_balance( const balance_id_type& balance_id );
         void remove_transaction( const transaction_id_type& record_id );

         vector<wallet_transaction_record> get_pending_transactions()const;

         owallet_transaction_record lookup_transaction( const transaction_id_type& record_id )const;

         map<private_key_type, string> get_account_private_keys( const fc::sha512& password )const;
         string                        get_account_name( const address& account_address )const;

         owallet_account_record lookup_account( const address& address_of_public_key )const;
         owallet_account_record lookup_account( const string& account_name )const;
         owallet_account_record lookup_account( account_id_type aid )const;

         vector<wallet_balance_record>  get_all_balances( const string& account_name, uint32_t limit );
         owallet_balance_record lookup_balance( const balance_id_type& balance_id )const;
         owallet_key_record     lookup_key( const address& address )const;


         owallet_setting_record   lookup_setting(const string& name)const;
         void                     store_setting(const string& name, const variant& value);

         bool has_private_key( const address& a )const;

         void remove_contact_account( const string& account_name);

         void rename_account( const public_key_type& old_account_key,
                              const string& new_account_name );

         void export_to_json( const path& filename )const;
         void import_from_json( const path& filename );

         bool                           validate_password( const fc::sha512& password )const;
         optional<extended_private_key> get_master_key( const fc::sha512& password    )const;
         void                           set_master_key( const extended_private_key& key,
                                                        const fc::sha512& new_password );

         void                           change_password( const fc::sha512& old_password,
                                                         const fc::sha512& new_password );

         const unordered_map< transaction_id_type, wallet_transaction_record >& get_transactions()const
         {
            return transactions;
         }
         const unordered_map< balance_id_type,wallet_balance_record >& get_balances()const
         {
            return balances;
         }
         const unordered_map< int32_t,wallet_account_record >& get_accounts()const
         {
            return accounts;
         }
         const unordered_map< address, wallet_key_record >& get_keys()const
         {
            return keys;
         }

         map<transaction_id_type, transaction_ledger_entry> experimental_transactions;

      private:
         optional<wallet_master_key_record>                               wallet_master_key;
         /** maps wallet_record_index to accounts */
         unordered_map< int32_t,wallet_account_record >                   accounts;
         unordered_map< address, wallet_key_record >                      keys;
         unordered_map< transaction_id_type, wallet_transaction_record >  transactions;
         unordered_map< balance_id_type,wallet_balance_record >           balances;
         map<property_enum, wallet_property_record>                       properties;
         map< string, wallet_setting_record >                             settings;

         unordered_map< address, address >                                btc_to_bts_address;
         unordered_map< address, int32_t >                                address_to_account_wallet_record_index;
         unordered_map< account_id_type, int32_t >                        account_id_to_wallet_record_index;
         map< string, int32_t >                                           name_to_account_wallet_record_index;

         void remove_item( int32_t index );

         template<typename T>
         void store_record( T& record_to_store )
         {
            if( record_to_store.wallet_record_index == 0 )
               record_to_store.wallet_record_index = new_wallet_record_index();
            store_generic_record( generic_wallet_record( record_to_store ) );
         }

        void store_generic_record( const generic_wallet_record& record );

        friend class detail::wallet_db_impl;
        unique_ptr<detail::wallet_db_impl> my;
   };

} } // bts::wallet
