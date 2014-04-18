using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;

namespace ImageSetGenerator
{
    public partial class Form1 : Form
    {

        class TreeNode
        {
            public ImageInfo image;
            public TreeNode LeftNode;
            public TreeNode RightNode;
            public Rectangle Rect=new Rectangle();

            public void DrawTo(Pen p,Graphics g)
            {
                g.DrawRectangle(p, Rect);
                if (LeftNode != null)
                    LeftNode.DrawTo(p, g);
                if (RightNode != null)
                    RightNode.DrawTo(p, g);
            }

            public bool Insert(ImageInfo img)
            {
                if (img.img.Width > Rect.Width ||
                    img.img.Height > Rect.Height)
                    return false;
                if (image == null)
                {
                    image = img;
                    image.targetRect.X = Rect.X;
                    image.targetRect.Y = Rect.Y;

                    image.targetRect.Width = image.img.Width;
                    image.targetRect.Height = image.img.Height;

                    int w = Rect.Width - image.img.Width;
                    int h = Rect.Height - image.img.Height;

                    Rectangle rect1 = new Rectangle(Rect.X + image.img.Width,
                        Rect.Y, w, image.img.Height);
                    Rectangle rect2 = new Rectangle(Rect.X ,
                        Rect.Y + image.img.Height, Rect.Width, h);

                    if (rect1.Width > 5 && rect1.Height> 5)
                    {
                        LeftNode = new TreeNode();
                        LeftNode.Rect = rect1;
                    }
                    if (rect2.Width > 5 && rect2.Height > 5)
                    {
                        RightNode = new TreeNode();
                        RightNode.Rect = rect2;
                    }

                    return true;
                }else
                {
                    if(LeftNode!=null)
                    {
                        if (LeftNode.Insert(img))
                            return true;
                    }
                    if(RightNode!=null)
                    {
                        if (RightNode.Insert(img))
                            return true;
                    }
                }
                return false;
            }

        }

        class ImageInfo
        {
            public ImageInfo(Image i,string p)
            {
                img = i;
                name = p;
                name=name.Replace(' ', '_');
                targetRect = new Rectangle();
            }

            public Image img;
            public string name;

            public Rectangle targetRect;

            public bool Compare(ImageInfo o)
            {
                if (img.Height > o.img.Height)
                    return true;
                if (img.Height == o.img.Height)
                {
                    if (name.CompareTo(o.name) == 1)
                        return true;
                    else return false;
                }
                return false;
            }
        }

        ImageInfo[] m_images ;

        public Form1()
        {
            InitializeComponent();
        }

        TreeNode root;


        bool TrySize(int width,int height)
        {
            root = new TreeNode();
            root.Rect.Width = width;
            root.Rect.Height = height;
            for (int i = 0; i < m_images.Length; ++i)
            {
                if (!root.Insert(m_images[i]))
                    return false;
            }
            return true;
        }

        Point EstimateSize()
        {
            int n = 7; //start with 128
            int best = n;
            Point res = new Point();
            for (; n < 11;++n )
            {
                res.X = (int)Math.Pow(2,n);
                res.Y = 0;
                int x = 0;
                int maxHeight = 0;
                bool ok = true;
                for (int i = 0; i < m_images.Length; ++i)
                {
                    if(x+m_images[i].img.Width>res.X)
                    {
                        if (x == 0 || res.Y > res.X)
                        {
                            ok = false;
                            break;
                        }
                        res.Y += maxHeight;
                        x = 0;
                        maxHeight = 0;
                    }
                    x += m_images[i].img.Width;
                    if(m_images[i].img.Height> maxHeight)
                    {
                        maxHeight=m_images[i].img.Height;
                    }
                }
                if (ok)
                {
                    res.Y += maxHeight;
                    break;
                }
            }
            for (int i = 1; i < 11;++i )
            {
                int Y = (int)Math.Pow(2, i);
                if(Y>res.Y)
                {
                    res.Y = Y;
                    break;
                }
            }

                return res;
        }

        void GenerateDebugTexture(string path)
        {
            Point sz = new Point();//= EstimateSize();
            sz.X = int.Parse(widthTxt.Text);
            sz.Y = int.Parse(heightTxt.Text);
            if (!TrySize(sz.X, sz.Y))
            {
                MessageBox.Show("Not Enough image size");
                return;
            }
            Bitmap result = new Bitmap(sz.X, sz.Y, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            Graphics g = Graphics.FromImage(result);
            g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
            g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
            g.Clear(Color.FromArgb(255, 255, 255, 0));
            Pen p = new Pen(Color.Black);
            root.DrawTo(p, g);
            g.Dispose();
            result.Save(path, System.Drawing.Imaging.ImageFormat.Png);
            result.Dispose();
        }

        void SaveImage(string path)
        {
            Point sz =new Point();//= EstimateSize();
            sz.X = int.Parse(widthTxt.Text);
            sz.Y = int.Parse(heightTxt.Text);
            if(!TrySize(sz.X,sz.Y))
            {
                MessageBox.Show("Not Enough image size");
                return;
            }
            Bitmap result = new Bitmap(sz.X, sz.Y,System.Drawing.Imaging.PixelFormat.Format32bppArgb);
        
            /*int x = 0;
            int y = 0;
            int maxHeight = 0;*/
            Graphics g = Graphics.FromImage(result);
            g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
            g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
            g.Clear(Color.FromArgb(0,0,0,0));
            for (int i = 0; i < m_images.Length; ++i)
            {/*
                if (x + m_images[i].img.Width > sz.X)
                {
                    y += maxHeight;
                    x = 0;
                    maxHeight = 0;
                }
                g.DrawImage(m_images[i].img, x, y, m_images[i].img.Width, m_images[i].img.Height);
                
                m_images[i].targetRect.X = x;
                m_images[i].targetRect.Y = y;
                m_images[i].targetRect.Width = m_images[i].img.Width;
                m_images[i].targetRect.Height = m_images[i].img.Height;
                x += m_images[i].img.Width;
                if (m_images[i].img.Height > maxHeight)
                {
                    maxHeight = m_images[i].img.Height;
                }*/
                g.DrawImage(m_images[i].img, m_images[i].targetRect.X, m_images[i].targetRect.Y,
                    m_images[i].targetRect.Width, m_images[i].targetRect.Height);
            }
            g.Dispose();
            result.Save(path, System.Drawing.Imaging.ImageFormat.Png);
            result.Dispose();
        }

        void SaveImageSet(string imageFile,string path)
        {
            int width;
            int height;

            width=int.Parse(widthTxt.Text);
            height=int.Parse(heightTxt.Text);
            XmlWriterSettings stg = new XmlWriterSettings();
            stg.Indent = true;
            XmlWriter w = XmlWriter.Create(path, stg);
            w.WriteStartElement("ImageSet");
            w.WriteAttributeString("Name", ImageSetNameTxt.Text);
            w.WriteAttributeString("TextureName", imageFile);
            for (int i = 0; i < m_images.Length;++i )
            {
                w.WriteStartElement("ImageRegion");
                w.WriteAttributeString("Name", m_images[i].name);
                string str = m_images[i].targetRect.X.ToString() + "," + m_images[i].targetRect.Y.ToString();
                w.WriteAttributeString("SourceRectPos", str);
                str = m_images[i].targetRect.Width.ToString() + "," + m_images[i].targetRect.Height.ToString();
                w.WriteAttributeString("SourceRectSize", str);

                float x;
                float y;

                x = m_images[i].targetRect.X / (float)width;
                y = m_images[i].targetRect.Y / (float)height;

                w.WriteAttributeString("TexcoordPos", x.ToString() + "," + y.ToString());

                x = m_images[i].targetRect.Width / (float)width;
                y = m_images[i].targetRect.Height / (float)height;

                w.WriteAttributeString("TexcoordSize", x.ToString() + "," + y.ToString());

                w.WriteEndElement();
            }
            w.WriteEndElement();
            w.Close();
        }

        void ExtractImages(string path)
        {
            string[] files = Directory.GetFiles(path, "*.png", SearchOption.TopDirectoryOnly);
            m_images = new ImageInfo[files.Length];
            for (int i = 0; i < files.Length;++i )
            {
                string fi = files[i];
                int idx=fi.LastIndexOf('\\');
                if (idx != -1)
                    fi = fi.substr(idx + 1);
                idx = fi.LastIndexOf('.');
                if (idx != -1)
                    fi = fi.substr(0,idx);

                m_images[i] = new ImageInfo(Image.FromFile(files[i]), fi);
            }

            //sort
            for(int i=0;i<m_images.Length;++i)
            {
                for (int j = i+1; j < m_images.Length; ++j)
                {
                    ImageInfo a = m_images[i];
                    ImageInfo b = m_images[j];
                    if(a.Compare(b)==false)
                    {
                        m_images[j] = a;
                        m_images[i] = b;
                    }
                }
            }

            SaveImage(path + "\\"+ImageSetNameTxt.Text+".png");
            SaveImageSet(ImageSetNameTxt.Text+".png",path + "\\"+ImageSetNameTxt.Text+".imageset");
            if(dbgChk.Checked)
                GenerateDebugTexture(path + "\\" + ImageSetNameTxt.Text + "_dbg.png");
            for (int i = 0; i < m_images.Length; ++i)
            {
                m_images[i].img.Dispose();
            }

            m_images = null;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            ExtractImages(PathTxt.Text);
        }

        private void button2_Click(object sender, EventArgs e)
        {
    
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.SelectedPath = PathTxt.Text;
            if(dlg.ShowDialog()==DialogResult.OK)
            {
                PathTxt.Text = dlg.SelectedPath;
            }
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            Divider d = new Divider();
            d.Show();
        }
    }
}