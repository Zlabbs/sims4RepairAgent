
using System.IO.Compression;
using System.Net.Http;
using System.Runtime.InteropServices;



namespace AgentScriptUpdater
{
    internal class Program
    {
        static string m_downloadDirectiory = "https://github.com/Zlabbs/sims4RepairAgent_Scripts/archive/refs/heads/main.zip";



        //main code
        static bool Download(string dowloadFrom, string dowloadTo)
        {
            using HttpClient networkHandler = new HttpClient();

            try
            {
                using Task<Stream> networkStreamData = networkHandler.GetStreamAsync(dowloadFrom);
                networkStreamData.Wait();

                new FileInfo(dowloadTo).Directory.Create();
                using var fileStream = new FileStream(dowloadTo, FileMode.Create, FileAccess.Write);

                networkStreamData.Result.CopyTo(fileStream);
                fileStream.Close();
            }
            catch
            {
                return false;
            }

            return true;
        }

        static bool Unzip(string unzipAt, string unzipTo)
        {
            try
            {
                Directory.Delete(unzipTo, true);
            }
            catch
            {

            }

            try
            {
                ZipFile.ExtractToDirectory(unzipAt, unzipTo);
            }
            catch
            {
                return false;
            }

            return true;
        }


        //main function
        static int Main(string[] args)
        {
            if (Download(m_downloadDirectiory, $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}/S4RA/raw.zip") == false)
            {
                return 1;
            }

            if (Unzip($"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}/S4RA/raw.zip", $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}/S4RA/out") == false)
            {
                return 2;
            }

            return 0;
        }
    }
}
