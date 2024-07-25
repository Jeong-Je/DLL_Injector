using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using DllInjectorLib;

namespace Wpf_DLL_Injector
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void BrowseButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "DLL files (*.dll)|*.dll|All files (*.*)|*.*";
            if (openFileDialog.ShowDialog() == true)
            {
                HookDllTextBox.Text = openFileDialog.FileName;
            }
        }

        private async void InjectButton_Click(object sender, RoutedEventArgs e)
        {
            string targetExePath = TargetExeTextBox.Text;
            string hookDllPath = HookDllTextBox.Text;

            if (string.IsNullOrEmpty(targetExePath) || string.IsNullOrEmpty(hookDllPath))
            {
                MessageBox.Show("타켓 EXE와 hook DLL을 모두 선택해 주세요.", "caution", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
            else
            {
                injectButton.IsEnabled = false;
                injectButton.Content = $"Waiting {targetExePath}...";
                try
                {
                    Injector injector = new Injector();
                    bool result = await Task.Run(() => injector.Inject(targetExePath, hookDllPath));

                    if (result)
                    {
                        MessageBox.Show("DLL 인젝션 성공", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                        injectButton.IsEnabled = true;
                        injectButton.Content = $"DLL 주입하기 💉";
                    }
                    else
                    {
                        MessageBox.Show("DLL 인젝션 실패", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                } catch (Exception ex)
                {
                    MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }
    }
}
