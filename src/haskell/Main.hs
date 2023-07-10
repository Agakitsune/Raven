module Handle where

import Parser
import Foreign.C.String (CString, peekCAString)
import Foreign.Ptr (nullPtr)
import System.Environment
import Data.Text.IO
import System.IO (hPutStrLn, stderr)
import Data.ByteString.Lazy.UTF8 (fromString)
import Data.Binary.Put
import Data.Word
import Text.Megaparsec
import Text.Megaparsec.Char (string, space1, newline)
import Text.Megaparsec.Char.Lexer
import Text.Megaparsec.Debug
import Data.ByteString.Lazy (ByteString, putStr, toStrict)
import Data.ByteString (useAsCString, length)

foreign export ccall ravenSerialize :: CString -> IO CString

ravenSerialize :: CString -> IO CString
ravenSerialize cfile = do
     (peekCAString cfile >>= Data.Text.IO.readFile) >>= \dat -> case parse ravenParser "" dat of
          (Left err) -> System.IO.hPutStrLn stderr (show $ errorBundlePretty err) >> pure nullPtr
          (Right decls) -> useAsCString (toStrict ((foldl (<>) (fromString "") $ map serialize (decls)) <> runPut (putWord8 255))) pure
