use crate::{
    buffer::{ResizeBuffer, SecretBytes, WriteBuffer},
    error::Error,
};

pub trait KeyAeadInPlace {
    /// Encrypt a secret value in place, appending the verification tag
    fn encrypt_in_place<B: ResizeBuffer>(
        &self,
        buffer: &mut B,
        nonce: &[u8],
        aad: &[u8],
    ) -> Result<(), Error>;

    /// Decrypt an encrypted (verification tag appended) value in place
    fn decrypt_in_place<B: ResizeBuffer>(
        &self,
        buffer: &mut B,
        nonce: &[u8],
        aad: &[u8],
    ) -> Result<(), Error>;

    /// Get the required nonce size for encryption
    fn nonce_size() -> usize;

    /// Get the size of the verification tag
    fn tag_size() -> usize;
}

pub trait KeyExchange<Rhs = Self> {
    fn key_exchange_buffer<B: WriteBuffer>(&self, other: &Rhs, out: &mut B) -> Result<(), Error>;

    fn key_exchange_bytes(&self, other: &Rhs) -> Result<SecretBytes, Error> {
        let mut buf = SecretBytes::with_capacity(128);
        self.key_exchange_buffer(other, &mut buf)?;
        Ok(buf)
    }
}

pub trait KeyFromExchange<Lhs, Rhs>: Sized {
    fn key_from_exchange(lhs: &Lhs, other: &Rhs) -> Result<Self, Error>;
}
