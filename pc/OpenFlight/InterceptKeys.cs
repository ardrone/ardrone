//==============================================================================
//http://blogs.msdn.com/b/toub/archive/2006/05/03/589423.aspx
//==============================================================================
//MSDN Blogs > Stephen Toub > Low-Level Keyboard Hook in C# 
//
//I answered a question today where someone asked for an example of setting a low-level 
//keyboard hook with C#. I actually have an example of doing so in my May 2006 MSDN 
//Magazine article on Managed Debugging Assistants, but the example is purposefully 
//buggy in order to demonstrate the behavior of certain MDAs. 
//
//Here is an example without the bug (compile this as a console application):
//==============================================================================
using System;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;

class InterceptKeys
{
    public delegate void KeyDownCallBack(int vkCode);
    private static KeyDownCallBack _callback;

    public static void Start(KeyDownCallBack callback)
    {
        _callback = callback;
        _hookID = SetHook(_proc);
    }
    public static void Stop()
    {
        UnhookWindowsHookEx(_hookID);
    }
    

    private const int WH_KEYBOARD_LL = 13;
    private const int WM_KEYDOWN = 0x0100;
    private static LowLevelKeyboardProc _proc = HookCallback;
    private static IntPtr _hookID = IntPtr.Zero;



    private static IntPtr SetHook(LowLevelKeyboardProc proc)
    {
        using (Process curProcess = Process.GetCurrentProcess())
        using (ProcessModule curModule = curProcess.MainModule)
        {
            return SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(curModule.ModuleName), 0);
        }
    }

    private delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, IntPtr lParam);

    private static IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
    {
        if (nCode >= 0 && wParam == (IntPtr)WM_KEYDOWN)
        {
            int vkCode = Marshal.ReadInt32(lParam);
            _callback(vkCode);
            //Console.WriteLine((Keys)vkCode + " " + vkCode.ToString());
        }
        return CallNextHookEx(_hookID, nCode, wParam, lParam);
    }

    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr SetWindowsHookEx(int idHook, LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);

    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool UnhookWindowsHookEx(IntPtr hhk);

    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr GetModuleHandle(string lpModuleName);
}
