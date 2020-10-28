use aries_askar::Result as KvResult;

mod utils;

macro_rules! db_tests {
    ($init:expr) => {
        use aries_askar::future::block_on;

        #[test]
        fn init() {
            block_on($init).unwrap();
        }

        #[test]
        fn fetch_fail() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_fetch_fail(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn add_fetch() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_add_fetch(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn add_fetch_tags() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_add_fetch_tags(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn count() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_count(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn scan() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_scan(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn create_lock_fail_non_existing() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_create_lock_fail_non_existing(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn create_lock_new_non_existing() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_create_lock_new_non_existing(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn create_lock_timeout() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_create_lock_timeout(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn create_lock_drop_expire() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_create_lock_drop_expire(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn keypair_create_fetch() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_keypair_create_fetch(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn keypair_sign_verify() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_keypair_sign_verify(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn keypair_pack_unpack_anon() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_keypair_pack_unpack_anon(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }

        #[test]
        fn keypair_pack_unpack_auth() {
            block_on(async {
                let db = $init.await?;
                super::utils::db_keypair_pack_unpack_auth(&db).await?;
                KvResult::Ok(())
            })
            .unwrap()
        }
    };
}

#[cfg(feature = "sqlite")]
mod sqlite {
    use super::*;
    use aries_askar::sqlite::{SqliteStore, SqliteStoreOptions};
    use aries_askar::{ProvisionStore, ProvisionStoreSpec, Store};

    async fn init_db() -> KvResult<Store<SqliteStore>> {
        let spec = ProvisionStoreSpec::create_default().await?;
        let db = SqliteStoreOptions::in_memory()
            .provision_store(spec)
            .await?;
        Ok(db)
    }

    db_tests!(init_db());

    #[test]
    fn provision_from_str() {
        block_on(async {
            let db_url = "sqlite://:memory:";
            let spec = ProvisionStoreSpec::create_default().await?;
            let _db = db_url.provision_store(spec).await?;
            KvResult::Ok(())
        })
        .unwrap();

        assert!(block_on(async {
            let db_url = "not-sqlite://test-db";
            let spec = ProvisionStoreSpec::create_default().await?;
            let _db = db_url.provision_store(spec).await?;
            KvResult::Ok(())
        })
        .is_err());
    }
}

#[cfg(all(feature = "pg_test", feature = "postgres"))]
mod postgres {
    use super::*;
    use aries_askar::postgres::{PostgresStore, TestDB};

    async fn init_db<'t>() -> KvResult<TestDB<'t>> {
        let db = PostgresStore::provision_test_db().await?;
        Ok(db)
    }

    db_tests!(init_db());
}
